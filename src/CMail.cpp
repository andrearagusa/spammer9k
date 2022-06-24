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
#include <ctime>
#include <sstream>
#include <iomanip>

#include "sha1/sha1.h"
#include "CMail.h"
#include "Utils.h"

std::string newline = "\r\n";

CMail::CMail()
{
    this->RawMailData.clear();
}

CMail::CMail(std::string To, std::string Subject, std::string Body, std::string Priority)
{
    this->To = To;
    this->Subject = Subject;
    this->Body = Body;

    this->Priority = Priority;

    this->RawMailData.clear();
}

CMail::CMail(std::string From, std::string To, std::string Subject, std::string Body, std::string Priority)
{
    this->From = From;
    this->To = To;
    this->Subject = Subject;
    this->Body = Body;

    this->Priority = Priority;

    this->RawMailData.clear();
}

void CMail::AddMailBody()
{
    RawMailData += newline;
    RawMailData += "--b1_" + GetMessageID() + newline;
    RawMailData += "Content-Type: text/plain; charset=us-ascii" + newline;
    RawMailData += newline;
    RawMailData += "To view the message, please use an HTML compatible email viewer!" + newline;
    RawMailData += newline;
    RawMailData += "--b1_" + GetMessageID() + newline;
    RawMailData += "Content-Type: text/html; charset=utf-8" + newline;
    RawMailData += "Content-Transfer-Encoding: base64" + newline;
    RawMailData += newline;
    RawMailData += Body + newline;
    RawMailData += newline;
    RawMailData += "--b1_" + GetMessageID() + "--" + newline;
    RawMailData += newline;
}

void CMail::AddNewsletterHeaders()
{
    //List-Unsubscribe: <mailto: emailAddress?subject=unsubscribe>, <unsubscribe URL >
    RawMailData += "Auto-Submitted: auto-generated" + newline;
}

void CMail::AddMailHeaders()
{
    RawMailData += "To: " + To + newline;
    RawMailData += "From: " + From + newline;
    RawMailData += "Reply-To: " + From + newline;
    RawMailData += "Message-ID: <" + GetMessageID() + "@" + From.substr(From.find("@") + 1) + ">" + newline;
    RawMailData += "Date: " + GetDate() + newline;
    RawMailData += "MIME-Version: 1.0" + newline;
    RawMailData += "Subject: " + Subject + newline;
    RawMailData += "Content-Type: multipart/alternative; boundary=\"b1_" + GetMessageID() + "\"" + newline;
    //RawMailData += "Content-Transfer-Encoding: 8bit" + newline;

}

void CMail::BuildMail(bool IsNewsletter)
{
    AddMailHeaders();
    if (IsNewsletter) {
        AddNewsletterHeaders();
    }
    AddMailBody();
}

std::string CMail::GetDate()
{
    std::stringstream CurTime;
    std::time_t t = std::time(nullptr);
    CurTime << std::put_time(std::localtime(&t), "%a, %d %b %Y %H:%M:%S %z");
    return CurTime.str();
}

std::string CMail::GetMessageID()
{
    unsigned char Digest[SHA1_DIGEST_LENGTH];
    std::string FormattedDigest;
    SHA1_CTX Context;

    time_t ts = time(0);

    SHA1Init(&Context);
    SHA1Update(&Context, (const unsigned char *) Body.c_str(), Body.length());
    SHA1Update(&Context, (const unsigned char *) To.c_str(), To.length());
    SHA1Update(&Context, (const unsigned char *) &ts, sizeof(ts));
    SHA1Final(&Context, Digest);

    stream2hex(Digest, SHA1_DIGEST_LENGTH, FormattedDigest, false);

    return FormattedDigest.substr(0, FormattedDigest.length());
}
