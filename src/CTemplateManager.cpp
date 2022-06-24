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

#include "CTemplateManager.h"

void CTemplateManager::AddTemplate(std::string TemplateName, std::string TemplateFolder)
{
    TemplateMap[TemplateName] = std::make_shared<CTemplate>(TemplateFolder);
}

bool CTemplateManager::CheckTemplate(std::string TemplateName)
{
    return TemplateMap.find(TemplateName) != TemplateMap.end();
}

CTemplate &CTemplateManager::GetTemplate(std::string TemplateName)
{
    return *TemplateMap[TemplateName];
}