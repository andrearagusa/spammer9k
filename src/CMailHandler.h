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
#include <queue>
#include <mutex>
#include <map>
#include <memory>

#include "CMail.h"

#pragma once

class CMailHandler
{
    typedef std::queue <CMail> tQueue;
    typedef std::shared_ptr <std::queue<CMail>> ptQueue;
public:

    CMailHandler(std::vector <std::string> Priorities = {DefaultPriority});

    //void PushMail(std::string From, std::string To, std::string Subject, std::string Body);
    void PushMail(std::string To, std::string Subject, std::string Body, std::string Priority = DefaultPriority);

    void PushMail(CMail &Mail);

    //void PushMail(std::string To, std::string Subject, std::string Body,);
    int GetMailCount();

    bool GetMail(CMail &Mail);

    bool HasMail();
    /*
    CMail GetMail();
    bool IsEmpty();
    void PopMail();
    void PushBackMail();
*/
private:
    static const std::string DefaultPriority;

    std::mutex QueueAccessMutex;
    //std::queue<CMail> *CurrentQueue;
    std::map<std::string, int> PriorityMap;
    std::map <std::string, ptQueue> QueueMap;
    std::vector <ptQueue> PriorityVector;

};


