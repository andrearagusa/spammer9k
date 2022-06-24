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

#include <string>
#include <vector>
#include <mutex>
#include <atomic>

#include "CProvider.h"
#include "CMail.h"

#pragma once

class CAccount
{
public:
    CAccount(std::string ServerAddress, int ServerPort, std::string Address, std::string Password, bool UseSSL = false);

    std::string GetServerAddress()  { return ServerAddress; };
    int GetServerPort()             { return ServerPort; };
    std::string GetAddress()        { return Address; };
    std::string GetPassword()       { return Password; };

    CProviderTable &GetProvider()   { return Providers; };

    void LockAccount()              { AccountMutex.lock(); };
    void UnlockAccount()            { AccountMutex.unlock(); };

    bool IsUseSSL()                 { return UseSSL; }

    int GetConsecutiveErrors()      { return ConsecutiveErrors; }
    void IncConsecutiveErrors()     { ConsecutiveErrors++; }
    void ResetConsecutiveErrors()   { ConsecutiveErrors = 0; }

private:
    std::string ServerAddress;
    int ServerPort;
    std::string Address;
    std::string Password;
    bool UseSSL;
    CProviderTable Providers;

    std::atomic<int> ConsecutiveErrors;
    std::mutex AccountMutex;
};

class CAccountManager
{
public:
    void AddAccount(std::string ServerAddress, int ServerPort, std::string Address, std::string Password, bool UseSSL = false);

    int GetAccountNumber()                  { return AccountList.size(); };
    CAccount *GetAccount(int AccountNum)    { return AccountList[AccountNum]; };

private:
    std::vector<CAccount *> AccountList;
};
