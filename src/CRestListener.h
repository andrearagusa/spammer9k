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

#include <cstdbool>
#include <map>
#include <tuple>

#include <cpprest/http_listener.h>
#include <cpprest/json.h>

#pragma once

extern bool g_Terminate;

class CRestListener
{
public:
    CRestListener(utility::string_t Endpoint);
    void Listen();
    void
    SetListenerHandler(web::http::method Method, std::function<void(web::http::http_request)> HandleMethodCallback);
    void SetSSLCert(utility::string_t SSLCertFile, utility::string_t SSLPrivKeyFile, utility::string_t SSLChainFile);
    void SetBasicAuthData(utility::string_t BasicAuthUsername, utility::string_t BasicAuthPassword);
    //std::tuple<utility::string_t, utility::string_t> GetBasicAuthData();
    bool IsBasicAuthEnabled() { return UseBasicAuth; };
    bool CheckBasicAuth(web::http::http_request Request);

private:
    std::unique_ptr <web::http::experimental::listener::http_listener> Listener;
    web::http::experimental::listener::http_listener_config Config;
    std::map <web::http::method, std::function<void(web::http::http_request)>> HandlerMap;
    utility::string_t Endpoint;
    //bool UseSSL;
    //utility::string_t SSLCert;
    //utility::string_t SSLPrivKey;
    //utility::string_t SSLChain;
    bool UseBasicAuth;
    //utility::string_t BasicAuthUsername;
    //utility::string_t BasicAuthPassword;
    utility::string_t BasicAuthString;
};