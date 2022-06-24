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
#include <map>
#include <experimental/filesystem>

class CConfig
{
public:
    struct AccountData
    {
        std::string Host;
        int Port;
        std::string Address;
        std::string Password;
        bool UseSSL;
    };

    struct TemplateData
    {
        std::string Name;
        std::string Path;
    };

    CConfig(const std::string &BasePath = ".", const std::string &ConfigFilename = "config.json");

    const std::vector<TemplateData> &GetTemplates()             { return TemplatesVector; }

    const std::vector<AccountData> &GetMailAccounts()           { return MailAccountsVector; }
    const std::vector<AccountData> &GetNewsletterAccounts()     { return NewsletterAccountsVector; }

    const std::vector<std::string> &GetPriorities()             { return MailPrioritiesVector; }

    bool GetUseBasicAuth()                                      { return UseBasicAuth; }
    std::string GetBasicAuthUsername()                          { return BasicAuthUsername; }
    std::string GetBasicAuthPassword()                          { return BasicAuthPassword; }

    std::string GetNewsletterEndpoint()                         { return NewsletterEndpoint; }
    std::string GetMailEndpoint()                               { return MailEndpoint; }
    std::string GetStatsEndpoint()                              { return StatsEndpoint; }
    std::string GetPingEndpoint()                               { return PingEndpoint; }

    //std::string GetLogFile()                                    { return LogFile; }

    std::string GetBasePath()                                   { return BasePath; }
    //void ReloadFile();

private:
    std::string BasePath;

    std::string NewsletterEndpoint;
    std::string MailEndpoint;
    std::string StatsEndpoint;
    std::string PingEndpoint;

    //std::string LogFile;

    bool UseBasicAuth;
    std::string BasicAuthUsername;
    std::string BasicAuthPassword;

    std::vector<TemplateData> TemplatesVector;

    std::vector<AccountData> MailAccountsVector;
    std::vector<AccountData> NewsletterAccountsVector;

    std::vector<std::string> MailPrioritiesVector;
};
