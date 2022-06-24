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
#include <experimental/filesystem>

#include "CLog.h"
#include "CConfig.h"
#include "nlohmann/json.hpp"

CConfig::CConfig(const std::string &BasePath, const std::string &ConfigFilename)
{
    std::string CurrentElem;
    nlohmann::json JsonData;

    this->BasePath = BasePath + "/";
    std::experimental::filesystem::path ConfigFile = this->BasePath + ConfigFilename;

    if (!std::experimental::filesystem::exists(ConfigFile))
    {
        LOG_ERROR() << "Config file not found: " << ConfigFile << std::endl;
        exit(1);
    }

    std::ifstream InFile(ConfigFile);

    try
    {
        InFile >> JsonData;

        NewsletterEndpoint = JsonData.at("newsletter_endpoint").get<std::string>();
        MailEndpoint = JsonData.at("mail_endpoint").get<std::string>();
        StatsEndpoint = JsonData.at("stats_endpoint").get<std::string>();
        PingEndpoint = JsonData.at("ping_endpoint").get<std::string>();

        //if (auto Elem = JsonData.find("log_file"); Elem != JsonData.end())
        //    LogFile = Elem->get<std::string>();

        if (auto Elem = JsonData.find("auth_info"); Elem != JsonData.end())
        {
            BasicAuthUsername = Elem->at("username").get<std::string>();
            BasicAuthPassword = Elem->at("password").get<std::string>();

            UseBasicAuth = true;
        } else
            UseBasicAuth = false;

        auto NewsletterTemplates = JsonData.at("newsletter_templates");
        if (NewsletterTemplates.empty())
        {
            LOG_ERROR() << "you must specify at least one template" << std::endl;
            exit(1);
        }
        for (auto const &Template : NewsletterTemplates)
        {
            TemplatesVector.push_back( {Template.at("name").get<std::string>(),
                    Template.at("folder").get<std::string>() } );
        }

        auto MailPriorities = JsonData.at("mail_priorities");
        if (MailPriorities.empty())
        {
            LOG_ERROR() << "you must specify at least one priority" << std::endl;
            exit(1);
        }
        for (auto const &Priority : MailPriorities) {
            MailPrioritiesVector.push_back(Priority.get<std::string>());
        }

        auto MailAccounts = JsonData.at("mail_accounts");
        if (MailAccounts.empty()) {
            LOG_ERROR() << "you must specify at least one mail account" << std::endl;
            exit(1);
        }
        for (auto const &Account : MailAccounts)
        {
            MailAccountsVector.push_back({Account.at("host").get<std::string>(),
                                          Account.at("port").get<int>(),
                                          Account.at("address").get<std::string>(),
                                          Account.at("password").get<std::string>(),
                                          Account.at("ssl").get<bool>()});
        }

        auto NewsletterAccounts = JsonData.at("newsletter_accounts");
        if (NewsletterAccounts.empty()) {
            LOG_ERROR() << "you must specify at least one newsletter account" << std::endl;
            exit(1);
        }
        for (auto const &Account : NewsletterAccounts)
        {
            NewsletterAccountsVector.push_back({Account.at("host").get<std::string>(),
                                                Account.at("port").get<int>(),
                                                Account.at("address").get<std::string>(),
                                                Account.at("password").get<std::string>(),
                                                Account.at("ssl").get<bool>()});
        }
    }

    catch (nlohmann::json::parse_error& e)
    {
        LOG_ERROR() << e.what() << std::endl;
        exit(1);
    }

    catch (nlohmann::json::out_of_range& e)
    {
        LOG_ERROR() << e.what() << std::endl;
        exit(1);
    }
}