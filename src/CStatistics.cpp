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

#include "CStatistics.h"

CStatistics::CStatistics()
{
    ResetStats();
}

void CStatistics::SendInit()
{
    StartTime = std::chrono::high_resolution_clock::now();
}

void CStatistics::ResetStats()
{
    StartTime = std::chrono::high_resolution_clock::now();
    TotalMails = 0;
    SentMails = 0;
}

double CStatistics::GetMailsPerSec()
{
    auto Now = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> Msec = Now - StartTime;

    return SentMails / Msec.count() * 1000;
}
