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

#include <csignal>

#include "CLog.h"
#include "CSigHandler.h"

volatile sig_atomic_t quitok = false;

void CSigHandler::GenericHandler(int signal)
{
    LOG_ERROR() << "Received signal " << signal;
}

CSigHandler::CSigHandler()
{
    //NOT IMPLEMENTED
    /*
    struct sigaction sigbreak;
    sigbreak.sa_handler = &CSigHandler::GenericHandler;
    sigemptyset(&sigbreak.sa_mask);
    sigbreak.sa_flags = 0;
    if (sigaction(SIGINT, &sigbreak, nullptr) != 0) std::perror("sigaction");
    if (sigaction(SIGTERM, &sigbreak, nullptr) != 0) std::perror("sigaction");
*/
    //std::signal(SIGINT, CSigHandler::GenericHandler);
    //std::signal(SIGTERM, CSigHandler::GenericHandler);
}