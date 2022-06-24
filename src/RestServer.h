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

#pragma once

#include <functional>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#include "CRestListener.h"
#include "CMailHandler.h"
#include "CTemplateManager.h"

typedef std::function<void(web::http::http_request)> tHandlerCallback;

tHandlerCallback GetMailPostCB(CRestListener &CurrentListener, const std::shared_ptr <CMailHandler> &CurrentHandler);
tHandlerCallback GetNewsletterPostCB(CRestListener &CurrentListener, const std::shared_ptr<CMailHandler> &CurrentHandler, CTemplateManager &TemplateManager);
tHandlerCallback PingGetCB(CRestListener &CurrentListener);

//extern CRestListener g_NewsletterEndpoint;
//extern CRestListener g_MailEndpoint;
//extern CRestListener g_StatsEndpoint;

//using namespace web::http::experimental::listener;

/*
class CRestServer
{
    CRestServer();

private:

    void SetListenerHandler();
    void StatusListenerHandler();
    web::http::experimental::listener::http_listener PushListener;
    web::http::experimental::listener::http_listener StatusListener;

};
*/