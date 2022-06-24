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
#include <map>
#include <chrono>
#include <mutex>

#include <cstring>
#include <cstdbool>
#include <ctime>

#include "sqlite3/sqlite3.h"

#pragma once

#define DEFAULT_DELAY 2000

struct SProviderInfo
{
public:

    SProviderInfo()
    {
        SendDelay = DEFAULT_DELAY;
    }

    SProviderInfo(unsigned int Delay)
    {
        SendDelay = Delay;
    }

    std::chrono::high_resolution_clock::time_point LastSendTime;
    unsigned int SendDelay;
};

class CSQLProvider
{
public:
    bool DBInit(std::string DBFile = NULL);
    bool LoadProviderData(std::map <std::string, SProviderInfo> &ProviderSendInfo);
    bool WriteProviderData(std::map <std::string, SProviderInfo> &ProviderSendInfo);

private:
    void BuildProviderTables();

    sqlite3 *Database;
};

class CProviderTable
{
public:
    CProviderTable();
    bool CanSendEmail(std::string Provider);
    void SendEmail(std::string Provider);
    bool AtomicSendEmail(std::string Provider);
    void FlushToDatabase();

private:
    std::map <std::string, SProviderInfo> ProviderSendInfo;
    CSQLProvider SQLDatabase;
    //std::mutex SendMailMutex;
};
