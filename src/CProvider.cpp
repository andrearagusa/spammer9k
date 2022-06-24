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

#include <iostream>
#include <cstdlib>
#include <ctime>

#include "CProvider.h"

static int
DelayCallback(std::map <std::string, SProviderInfo> &ProviderSendInfo, int count, char **data, char **columns)
{
    if (count == 2)
        ProviderSendInfo.insert({data[0], strtol(data[1], NULL, 10)});

    return 0;
}

bool CSQLProvider::DBInit(std::string SQLDBFile)
{
    int rc;

    if (SQLDBFile.length()) {
        rc = sqlite3_open(SQLDBFile.c_str(), &Database);
        BuildProviderTables();
    } else {
        rc = sqlite3_open(":memory:", &Database);
    }

    if (rc != SQLITE_OK) {
        printf("An SQLite error occurrect, ec: %d\n", rc);
        return true;
    } else {
        return false;
    }
}

void CSQLProvider::BuildProviderTables()
{
    char *ErrMsg = NULL;
    char Statement[] = "CREATE TABLE IF NOT EXISTS Providers"  \
                       "(Name TEXT PRIMARY KEY     NOT NULL," \
                       "Delay           INT    NOT NULL);";

    sqlite3_exec(Database, Statement, NULL, 0, &ErrMsg);
}

bool CSQLProvider::LoadProviderData(std::map <std::string, SProviderInfo> &ProviderSendInfo)
{
    char *ErrMsg = NULL;
    char Statement[] = "SELECT Name, Delay FROM Providers";

    sqlite3_exec(Database, Statement, (int (*)(void *, int, char **, char **)) DelayCallback, &ProviderSendInfo,
                 &ErrMsg);

    if (ErrMsg) {
        return true;
    } else {
        return false;
    }
}

bool CSQLProvider::WriteProviderData(std::map <std::string, SProviderInfo> &ProviderSendInfo)
{
    char *ErrMsg = NULL;
    std::string Query;

    if (!Database) {
        return true;
    }

    for (auto &Iterator : ProviderSendInfo) {
        Query += "INSERT OR IGNORE INTO Providers (Name, Delay) VALUES ('" + Iterator.first + "', " +
                 std::to_string(Iterator.second.SendDelay) + ");";
        Query += "UPDATE Providers SET Delay = " + std::to_string(Iterator.second.SendDelay) + " WHERE Name = '" +
                 Iterator.first + "';";
    }

    sqlite3_exec(Database, Query.c_str(), NULL, 0, &ErrMsg);

    return false;
}

CProviderTable::CProviderTable()
{
    SQLDatabase.DBInit("ProvDB.sqlite");
    SQLDatabase.LoadProviderData(ProviderSendInfo);
}

bool CProviderTable::CanSendEmail(std::string Provider)
{
    SProviderInfo ProvInfo = ProviderSendInfo[Provider];
    std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> MilliSeconds = Now - ProvInfo.LastSendTime;

    if (MilliSeconds.count() >= ProvInfo.SendDelay) {
        return true;
    } else {
        return false;
    }
}

bool CProviderTable::AtomicSendEmail(std::string Provider)
{
    SProviderInfo ProvInfo = ProviderSendInfo[Provider];
    std::chrono::high_resolution_clock::time_point Now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> MilliSeconds = Now - ProvInfo.LastSendTime;

    if (MilliSeconds.count() >= ProvInfo.SendDelay) {
        ProvInfo.LastSendTime = std::chrono::high_resolution_clock::now();
        ProviderSendInfo[Provider] = ProvInfo;
        return true;
    } else {
        return false;
    }

}

void CProviderTable::SendEmail(std::string Provider)
{
    SProviderInfo ProvInfo = ProviderSendInfo[Provider];
    ProvInfo.LastSendTime = std::chrono::high_resolution_clock::now();
    ProviderSendInfo[Provider] = ProvInfo;
}

void CProviderTable::FlushToDatabase()
{
    //broken for mt implementation
    //SQLDatabase.WriteProviderData(ProviderSendInfo);
}
