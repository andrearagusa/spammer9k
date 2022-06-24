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

#include <vector>
#include <string>

#include "CLog.h"
#include "CConfig.h"
#include "CTemplateManager.h"
#include "CMailDispatcher.h"
#include "CAccount.h"
#include "CMailHandler.h"
#include "CRestListener.h"
#include "RestServer.h"
#include "CSigHandler.h"

#include "argparse/argparse.h"

bool g_Terminate = false;

int main(int argc, char *argv[])
{
    std::string BasePath = ".";
    std::string ConfigFilename = "config.json";

    ArgumentParser Parser("Parser");
    Parser.add_argument("-b","--base_path" , "Base path, where all the other files lies (default: current folder)", false);
    Parser.add_argument("-c","--conf_file", "Config file name (default: config.json)", false);
    Parser.add_argument("-l","--log_file", "Log file (default: none)", false);

    try {
        Parser.parse(argc, argv);
    } catch (const ArgumentParser::ArgumentNotFound& ex) {
        LOG_ERROR() << ex.what() << std::endl;
        return 0;
    }

    if (Parser.exists("b"))
        BasePath = Parser.get<std::string>("b");
    if (Parser.exists("c"))
        ConfigFilename = Parser.get<std::string>("c");
    if (Parser.exists("l"))
        CLog::SetFileLogging(Parser.get<std::string>("l"));
        //BasePath = Parser.get<std::string>("l");

    CConfig Config(BasePath, ConfigFilename);
    //if (!Config.GetLogFile().empty())
    //    CLog::SetFileLogging(Config.GetBasePath() + Config.GetLogFile());

    CSigHandler SigHandler;
    CTemplateManager TemplateManager;
    //std::vector <std::string> PriorityLevels = {"HIGH", "NORMAL", "LOW"};

    auto MailHandler = std::make_shared<CMailHandler>(Config.GetPriorities());
    auto NewsletterHandler = std::make_shared<CMailHandler>();

    auto NewsletterAccountMgr = std::make_shared<CAccountManager>();
    auto MailAccountMgr = std::make_shared<CAccountManager>();

    //set up accounts
    for (const auto &Account : Config.GetNewsletterAccounts())
        NewsletterAccountMgr->AddAccount(Account.Host, Account.Port, Account.Address, Account.Password, Account.UseSSL);

    for (const auto &Account : Config.GetMailAccounts())
        MailAccountMgr->AddAccount(Account.Host, Account.Port, Account.Address, Account.Password, Account.UseSSL);

    //initialize template manager
    for (const auto &Template : Config.GetTemplates())
        TemplateManager.AddTemplate(Template.Name, Config.GetBasePath() + Template.Path);

    //initialize endpoints
    CRestListener NewsletterEndpoint(Config.GetNewsletterEndpoint());
    CRestListener MailEndpoint(Config.GetMailEndpoint());
    CRestListener StatsEndpoint(Config.GetStatsEndpoint());
    CRestListener PingEndpoint(Config.GetPingEndpoint());

    //initialize and start queue dispatchers
    CMailDispatcher NewsletterDispatcher(NewsletterHandler, NewsletterAccountMgr, true, 5);
    CMailDispatcher MailDispatcher(MailHandler, MailAccountMgr, false, 5);

    NewsletterDispatcher.Start();
    MailDispatcher.Start();

    //Set REST callbacks
    auto MailPostHandler = GetMailPostCB(MailEndpoint, MailHandler);
    MailEndpoint.SetListenerHandler(web::http::methods::POST, MailPostHandler);
    if (Config.GetUseBasicAuth())
        MailEndpoint.SetBasicAuthData(U(Config.GetBasicAuthUsername()), U(Config.GetBasicAuthPassword()));

    auto NewsletterPostHandler = GetNewsletterPostCB(NewsletterEndpoint, NewsletterHandler, TemplateManager);
    NewsletterEndpoint.SetListenerHandler(web::http::methods::POST, NewsletterPostHandler);
    if (Config.GetUseBasicAuth())
        NewsletterEndpoint.SetBasicAuthData(U(Config.GetBasicAuthUsername()), U(Config.GetBasicAuthPassword()));

    auto PingGetHandler = PingGetCB(PingEndpoint);
    PingEndpoint.SetListenerHandler(web::http::methods::GET, PingGetHandler);
    if (Config.GetUseBasicAuth())
        PingEndpoint.SetBasicAuthData(U(Config.GetBasicAuthUsername()), U(Config.GetBasicAuthPassword()));

    //start REST listeners
    MailEndpoint.Listen();
    NewsletterEndpoint.Listen();
    PingEndpoint.Listen();

}
