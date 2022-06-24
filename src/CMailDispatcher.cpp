/*******************************************************************
Copyright (C) 2018 2019 2020 Andrea Ragusa
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
********************************************************************/

#include <sstream>
#include <thread>
#include <algorithm>

#include <curl/curl.h>

#include "CStatistics.h"
#include "CMailDispatcher.h"
#include "CLog.h"
#include "Utils.h"

struct cUrlMailData
{
    CMail *Mail;
    unsigned int readBytes;
};

static size_t ReadDataCallback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    cUrlMailData *MailData = (cUrlMailData *) userp;

    if ((size == 0) || (nmemb == 0) || ((size * nmemb) < 1))
        return 0;

    if (MailData->readBytes >= MailData->Mail->GetRawMailData().length())
        return 0;

    strncpy((char *) ptr, MailData->Mail->GetRawMailData().c_str() + MailData->readBytes, nmemb * size);
    MailData->readBytes += MailData->Mail->GetRawMailData().length();

    return MailData->Mail->GetRawMailData().length();
}

static int DebugCallback(CURL *handle, curl_infotype type, char *data, size_t size, void *userp)
{
    static const char s_infotype[CURLINFO_END][3] = {"* ", "< ", "> ", "{ ", "} ", "{ ", "} " };

    switch (type)
    {
        case CURLINFO_TEXT:
        case CURLINFO_HEADER_OUT:
        case CURLINFO_HEADER_IN:
            LOG_DEBUG() << "CMailDispatcher send log: " << s_infotype[type] << std::string(data).erase(size);
            break;
            
        default:
            break;
    }
    return 0;
}

CMailDispatcher::CMailDispatcher(std::shared_ptr <CMailHandler> &MailHandler,
                                 std::shared_ptr <CAccountManager> &AccountManager,
                                 bool EnableStats,
                                 int ThreadsPerAccount)
{
    this->ThreadsPerAccount = ThreadsPerAccount;
    this->MailHandler = MailHandler;
    this->AccountManager = AccountManager;
    this->EnableStats = EnableStats;
}

int CMailDispatcher::ThreadProc_cUrlSendMail(CAccount *Account, const std::shared_ptr<bool> &Done, bool Verbose)
{
    CMail Mail;

    std::string ServerAddress = Account->GetServerAddress();
    int ServerPort = Account->GetServerPort();
    std::string Username = Account->GetAddress();
    std::string Password = Account->GetPassword();

    cUrlMailData MailData = {&Mail, 0};
    CURL *curl;

    CURLcode res = CURLE_OK;
    struct curl_slist *recipients = NULL;
    std::stringstream ServerConnectString;
    ServerConnectString << "smtp://" << ServerAddress << ":" << ServerPort;

    curl = curl_easy_init();

    if (Account->IsUseSSL())
    {
        curl_easy_setopt(curl, CURLOPT_SSLVERSION, (long) CURL_SSLVERSION_DEFAULT);
        curl_easy_setopt(curl, CURLOPT_USE_SSL, (long) CURLUSESSL_ALL);
    }

    curl_easy_setopt(curl, CURLOPT_USERNAME, Username.c_str());
    curl_easy_setopt(curl, CURLOPT_PASSWORD, Password.c_str());
    curl_easy_setopt(curl, CURLOPT_URL, ServerConnectString.str().c_str());
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    curl_easy_setopt(curl, CURLOPT_DEBUGFUNCTION, DebugCallback);
    curl_easy_setopt(curl, CURLOPT_VERBOSE, (int) Verbose);

    curl_easy_setopt(curl, CURLOPT_READFUNCTION, ReadDataCallback);
    curl_easy_setopt(curl, CURLOPT_READDATA, &MailData);

    while (GetNextMail(*Account, Mail))
    {
        if ((Account->GetConsecutiveErrors() % 5 == 0) && (Account->GetConsecutiveErrors() > 0) )
        {
            //every 5 consecutive errors, delay the loop by some time;
            LOG_DEBUG() << "Can't send, delaying by " << 5 << " minutes";
            std::this_thread::sleep_for(std::chrono::minutes(5));
        }

        MailData.readBytes = 0;
        Mail.SetFrom(Account->GetAddress());
        Mail.BuildMail(true);

        recipients = curl_slist_append(NULL, Mail.GetTo());

        curl_easy_setopt(curl, CURLOPT_MAIL_FROM, Mail.GetFrom());
        curl_easy_setopt(curl, CURLOPT_MAIL_RCPT, recipients);

        res = curl_easy_perform(curl);

        curl_slist_free_all(recipients);

        if (res != CURLE_OK)
        {
            LOG_ERROR() << "curl_easy_perform() failed: " << curl_easy_strerror(res) << std::endl;
            Account->IncConsecutiveErrors();
            MailHandler->PushMail(Mail);
        }
        else
        {
            Account->ResetConsecutiveErrors();

            if (EnableStats)
                CStatistics::instance().Send();
        }
    }

    curl_easy_cleanup(curl);
    *Done = true;
    return res;
}


bool CMailDispatcher::GetNextMail(CAccount &Account, CMail &Mail)
{
    std::string Provider;

    while (MailHandler->GetMail(Mail))
    {
        Account.LockAccount();

        Provider = Mail.GetProvider();

        if (Account.GetProvider().CanSendEmail(Provider))
        {
            Account.GetProvider().SendEmail(Provider);
            Account.UnlockAccount();
            return true;
        }
        else
        {
            MailHandler->PushMail(Mail);
            Account.UnlockAccount();
            std::this_thread::sleep_for(std::chrono::milliseconds(50));
        }
    }
    return false;
}

void CMailDispatcher::ThreadProc_DispatcherLoop()
{
    while (!g_Terminate)
    {
        if (MailHandler->HasMail())
        {
            while (ThreadMap.size() < std::min<unsigned int>(AccountManager->GetAccountNumber() * ThreadsPerAccount, MailHandler->GetMailCount()))
            {
                std::shared_ptr<bool> TempBool = std::make_shared<bool>(false);
                std::shared_ptr <std::thread> TempThread = std::make_shared<std::thread>(
                        &CMailDispatcher::ThreadProc_cUrlSendMail, this,
                        AccountManager->GetAccount(ThreadMap.size() % AccountManager->GetAccountNumber()), TempBool, true);

                ThreadMap.insert({TempThread, TempBool});
            }

            for (auto Thread = ThreadMap.begin(); Thread != ThreadMap.end();)
            {
                if ((*Thread->second) == true)
                {
                    Thread->first->join();
                    Thread = ThreadMap.erase(Thread);
                }
                else
                    Thread++;
            }

        }
        else if (EnableStats)
            CStatistics::instance().ResetStats();

        std::this_thread::sleep_for(std::chrono::milliseconds(1000));

    }
}

CMailDispatcher::~CMailDispatcher()
{
    MainThread->join();
    curl_global_cleanup();
};

void CMailDispatcher::Start()
{
    curl_global_init(CURL_GLOBAL_ALL);
    MainThread = std::make_unique<std::thread>(&CMailDispatcher::ThreadProc_DispatcherLoop, this);
}
