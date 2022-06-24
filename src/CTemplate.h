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
#include <experimental/filesystem>

#include <inja/inja.hpp>

#ifndef CTEMPLATE_H
#define CTEMPLATE_H

class CTemplate
{
public:
    CTemplate(std::experimental::filesystem::path TemplateFolder);
    std::string Render(const std::string &NewsletterInfos);

private:
    nlohmann::json &LoadJSON(std::experimental::filesystem::path Path);
    std::experimental::filesystem::path TemplateFolder;

    nlohmann::json NewsletterData;
    inja::Environment TemplateEnv;
    nlohmann::json Structure;
};

#endif //CTEMPLATE_H
