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
#include "CMailHandler.h"

const std::string CMailHandler::DefaultPriority = "NORMAL";

CMailHandler::CMailHandler(std::vector <std::string> Priorities)
{
    int k = 0;

    if (Priorities.size() < 1)
    {
        //error!
    }

    PriorityVector.resize(Priorities.size());

    for (auto const Prio : Priorities)
    {
        auto Queue = std::make_shared<tQueue>();
        PriorityMap[Prio] = k;
        QueueMap[Prio] = Queue;
        PriorityVector[k] = Queue;
        k++;
    }
}

void CMailHandler::PushMail(std::string To, std::string Subject, std::string Body, std::string Priority)
{
    std::lock_guard <std::mutex> lock(QueueAccessMutex);

    //check that QueueMap[Priority] actually exists

    QueueMap[Priority]->emplace(To, Subject, Body, Priority);
/*
    if (Priority.length())
        CurrentQueue = &QueueMap[Priority];
    else
        Priority = DefaultPriority;

    CurrentQueue->emplace(To, Subject, Body, Priority);
    */
}

void CMailHandler::PushMail(CMail &Mail)
{
    std::lock_guard <std::mutex> lock(QueueAccessMutex);

    //check that QueueMap[Mail.GetPriority()] actually exists
    /*
    if (Mail.GetPriority().length())
    {
        CurrentQueue = &QueueMap[Mail.GetPriority()];
        Priority = Mail.GetPriority();
    }

    CurrentQueue->push(Mail);
*/
    QueueMap[Mail.GetPriority()]->push(Mail);
}

bool CMailHandler::GetMail(CMail &Mail)
{
    std::lock_guard <std::mutex> lock(QueueAccessMutex);

    for (auto const Queue : PriorityVector)
    {
        if (Queue->size())
        {
            Mail = Queue->front();
            Queue->pop();
            return true;
        }
    }
    return false;
}

bool CMailHandler::HasMail()
{
    std::lock_guard <std::mutex> lock(QueueAccessMutex);

    for (auto const Queue : PriorityVector)
    {
        if (Queue->size())
            return true;
    }
    return false;
}

int CMailHandler::GetMailCount()
{
    std::lock_guard <std::mutex> lock(QueueAccessMutex);
    int Size = 0;

    for (auto const Queue : PriorityVector)
        Size += Queue->size();

    return Size;
}
/*
CMail CMailHandler::GetMail()
{
    return MailQueue.front();
}

bool CMailHandler::IsEmpty()
{
    return MailQueue.empty();
}

void CMailHandler::PopMail()
{
    MailQueue.pop();
}

void CMailHandler::PushBackMail()
{
    MailQueue.push(MailQueue.front());
    MailQueue.pop();
}
*/