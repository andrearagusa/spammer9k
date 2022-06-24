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
#include <map>

#include "CTemplate.h"

#ifndef CTEMPLATEMANAGER_H
#define CTEMPLATEMANAGER_H

class CTemplateManager
{
public:
    //CTemplateManager();

    void AddTemplate(std::string TemplateName, std::string TemplateFolder);
    CTemplate &GetTemplate(std::string TemplateName);
    bool CheckTemplate(std::string TemplateName);

private:
    std::map<std::string, std::shared_ptr<CTemplate>> TemplateMap;
};

#endif //CTEMPLATEMANAGER_H
