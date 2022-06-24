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

#pragma once

class CMail
{
public:
    CMail();
    CMail(std::string From, std::string To, std::string Subject, std::string Body, std::string Priority);
    CMail(std::string To, std::string Subject, std::string Body, std::string Priority);
    void BuildMail(bool IsNewsletter);

    std::string GetRawMailData()        { return RawMailData; }
    void SetFrom(std::string From)      { this->From = From; }
    const char *GetFrom()               { return From.c_str(); }
    const char *GetTo()                 { return To.c_str(); }
    const char *GetBody()               { return Body.c_str(); }
    const char *GetSubject()            { return Subject.c_str(); }
    std::string GetProvider()           { return To.substr(To.find("@") + 1); }
    std::string GetPriority()           { return Priority; }

private:
    void AddMailBody();
    void AddNewsletterHeaders();
    void AddMailHeaders();
    std::string GetDate();
    std::string GetMessageID();

    std::string Priority;

    std::string From;
    std::string To;
    std::string Body;
    std::string Subject;

    std::string RawMailData;
};
