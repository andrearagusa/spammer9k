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
#include <fstream>
#include <sstream>
#include <string>
#include <mutex>
#include <experimental/filesystem>

#ifndef _CLOG_H_
#define _CLOG_H_

// Direct Interface for logging into log file or console using MACRO(s)
#define LOG_ERROR()    CLog(LOG_LEVEL_ERROR) //<< __FUNCTION__ << " "
#define LOG_INFO()     CLog(LOG_LEVEL_INFO)
#define LOG_TRACE()    CLog(LOG_LEVEL_TRACE)
#define LOG_DEBUG()    CLog(LOG_LEVEL_DEBUG) //<< __FUNCTION__ << " "

typedef enum LOG_LEVEL
{
    DISABLE_LOG         = 1,
    LOG_LEVEL_INFO	    = 2,
    LOG_LEVEL_TRACE     = 3,
    LOG_LEVEL_DEBUG     = 4,
    LOG_LEVEL_ERROR     = 5,
    ENABLE_LOG          = 6,
} ELogLevel;

class CLog
{
public:
    CLog(enum LOG_LEVEL level);
    void PrependTimestamp();

    // Interfaces to control log levels
    static void SetLogLevel(ELogLevel logLevel) { LogLevel = logLevel; }
    static void EnaleLog() { LogLevel = ENABLE_LOG; }  // Enable all log levels
    static void DisableLog() { LogLevel = DISABLE_LOG; } // Disable all log levels

    // Interfaces to control log Types
    static void SetConsoleLogging();
    static void SetFileLogging(std::experimental::filesystem::path LogFilePath);

    template<typename T>
    CLog &operator<<(T const &t)
    {
        st << t;
        return *this;
    }

    CLog &operator<<(std::ostream &(*f)(std::ostream &))
    {
        st << f;
        return *this;
    }

    ~CLog();

private:
    void operator=(const CLog& obj) {}
    std::string getCurrentTime();

private:
    std::stringstream                              st;
    ELogLevel                                      CurLogLevel;

    static std::ostream                            *OutStream;
    static ELogLevel                               LogLevel;

    static std::ofstream                           FileStream;
};

#endif // End of _CLOG_H_

