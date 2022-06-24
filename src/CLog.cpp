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
#include <ctime>
#include <iomanip>
#include <algorithm>

#include "CLog.h"

ELogLevel CLog::LogLevel = ENABLE_LOG;
std::ostream *CLog::OutStream = &std::cout;
std::ofstream CLog::FileStream;

CLog::~CLog()
{
    std::string Line;
    while (std::getline(st, Line))
    {
        //Line.erase(std::remove(Line.begin(), Line.end(), '\r'), Line.end());

        PrependTimestamp();
        *OutStream << Line << std::endl;
    }
}

void CLog::SetConsoleLogging()
{
    if (FileStream.is_open())
        FileStream.close();

    OutStream = &std::cout;
}

void CLog::SetFileLogging(std::experimental::filesystem::path LogFilePath)
{
    if (FileStream.is_open())
        FileStream.close();

    FileStream.open(LogFilePath, std::ios::out | std::ios::app);
    if (!FileStream.is_open())
        CLog(LOG_LEVEL_ERROR) << "Cannot open log file: " << LogFilePath << std::endl;
    else
        OutStream = &FileStream;
}

std::string CLog::getCurrentTime()
{
    std::stringstream CurTime;
    std::time_t t = std::time(nullptr);
    CurTime << std::put_time(std::localtime(&t), "%D %T");
    return CurTime.str();
}

CLog::CLog(enum LOG_LEVEL level)
{
    CurLogLevel = level;
}

void CLog::PrependTimestamp()
{
    *OutStream << getCurrentTime() << " - ";

    if (CurLogLevel == LOG_LEVEL_ERROR)
        *OutStream << "[ERROR]: ";
    else if (CurLogLevel == LOG_LEVEL_INFO)
        *OutStream << "[INFO]: ";
    else if (CurLogLevel == LOG_LEVEL_TRACE)
        *OutStream << "[TRACE]: ";
    else if (CurLogLevel == LOG_LEVEL_DEBUG)
        *OutStream << "[DEBUG]: ";
}