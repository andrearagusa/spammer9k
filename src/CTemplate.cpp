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

#include <inja/inja.hpp>

#include "CTemplate.h"
#include "CLog.h"

nlohmann::json &CTemplate::LoadJSON(std::experimental::filesystem::path Path)
{
    static nlohmann::json JsonData;
    JsonData.clear();

    if (!std::experimental::filesystem::exists(Path))
    {
        LOG_ERROR() << "Config file not found: " << Path << std::endl;
        exit(1);
    }

    std::ifstream InFile(Path);

    InFile >> JsonData;
    return JsonData;
}

void SetCallbacks(inja::Environment &TemplateEnv, nlohmann::json &NewsletterData)
{
    TemplateEnv.add_callback("set_elements", 1, [&NewsletterData](inja::Arguments &args)
    {
        NewsletterData["elements"] = args.at(0)->get<nlohmann::json>();
        return "";
    });

    TemplateEnv.add_callback("get_element", 2, [&TemplateEnv, &NewsletterData](inja::Arguments &args)
    {
        //std::string Out;
        auto file = args.at(0)->get<std::string>();

        int elemId = args.at(1)->get<int>();
        NewsletterData["element"] = NewsletterData.at("elements").at(elemId);
        inja::Template temp = TemplateEnv.parse_template(file);
        //Out = TemplateEnv.render(temp, NewsletterData);
        return TemplateEnv.render(temp, NewsletterData);
    });
}

CTemplate::CTemplate(std::experimental::filesystem::path TemplateFolder)
{
    this->TemplateFolder = TemplateFolder;
    Structure = LoadJSON(TemplateFolder / "structure.json");
    TemplateEnv = inja::Environment(TemplateFolder);
    SetCallbacks(TemplateEnv, NewsletterData);
}

std::string CTemplate::Render(const std::string &NewsletterInfos)
{
    nlohmann::json JsonData;
    std::string result;
    try
    {
        JsonData = nlohmann::json::parse(NewsletterInfos);


        std::string lang = JsonData.at("language").get<std::string>();
        NewsletterData["newsletter"] = JsonData;
        NewsletterData["translate"] = Structure.at("translations").at(lang);

        inja::Template temp = TemplateEnv.parse_template("/base.html");
        result = TemplateEnv.render(TemplateEnv.render(temp, NewsletterData),NewsletterData);
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

    return result;
}