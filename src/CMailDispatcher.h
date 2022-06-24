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
#include <thread>
#include <memory>
#include <atomic>

#include "CMailHandler.h"
#include "CAccount.h"

#pragma once

extern bool g_Terminate;

class CMailDispatcher
{
public:
    CMailDispatcher(std::shared_ptr <CMailHandler> &MailHandler, std::shared_ptr <CAccountManager> &AccountManager,
                    bool EnableStats = false, int ThreadsPerAccount = 5);

    ~CMailDispatcher();

    void Start();

private:
    bool GetNextMail(CAccount &Account, CMail &Mail);

    int ThreadProc_cUrlSendMail(CAccount *Account, const std::shared_ptr<bool> &Done, bool Verbose = false);

    void ThreadProc_DispatcherLoop();

    std::shared_ptr <CMailHandler> MailHandler;
    std::shared_ptr <CAccountManager> AccountManager;

    std::shared_ptr <std::thread> MainThread;
    int ThreadsPerAccount;
    //std::vector<std::shared_ptr<std::thread>> ThreadVector;
    std::map <std::shared_ptr<std::thread>, std::shared_ptr<bool>> ThreadMap;
    //std::atomic<int> ThreadCount;
    bool EnableStats;
};