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

#include "CRestListener.h"

#include <utility>

CRestListener::CRestListener(utility::string_t Endpoint)
{
    UseBasicAuth = false;
    //UseSSL = false;
    this->Endpoint = Endpoint;
    Listener = nullptr;
}

void CRestListener::Listen()
{
    if (Listener == nullptr) {
        Listener = std::make_unique<web::http::experimental::listener::http_listener>(Endpoint, Config);
    }

    for (const auto &Handler : HandlerMap)
        Listener->support(Handler.first, Handler.second);

    try {
        Listener->open().wait();//.then([&]() { std::cout << U("\nstarting to listen\n"); }).wait();
        //while (!g_Terminate) {}
    }
    catch (std::exception const &e) {
        std::cout << e.what() << std::endl;
    }
}

void
CRestListener::SetListenerHandler(web::http::method Method, std::function<void(web::http::http_request)> HandleCallback)
{
    HandlerMap[Method] = HandleCallback;
}

void CRestListener::SetSSLCert(utility::string_t SSLCertFile, utility::string_t SSLPrivKeyFile,
                               utility::string_t SSLChainFile)
{
    if (!SSLCertFile.length() && !SSLChainFile.length() && !SSLPrivKeyFile.length()) {
        return;
    }

    Config.set_ssl_context_callback([SSLCertFile, SSLPrivKeyFile, SSLChainFile](boost::asio::ssl::context &ctx)
    {
        ctx.set_options(boost::asio::ssl::context::default_workarounds);

        // Password callback needs to be set before setting cert and key.
        /*
        ctx.set_password_callback([](std::size_t max_length, boost::asio::ssl::context::password_purpose purpose)
        {
        return "password";
        });
        */
        ctx.use_certificate_file(SSLCertFile, boost::asio::ssl::context::pem);
        ctx.use_private_key_file(SSLPrivKeyFile, boost::asio::ssl::context::pem);
        ctx.use_certificate_chain_file(SSLChainFile);
    });

}

void CRestListener::SetBasicAuthData(utility::string_t BasicAuthUsername, utility::string_t BasicAuthPassword)
{
    utility::string_t AuthString = U(BasicAuthUsername + ":" + BasicAuthPassword);
    std::vector<unsigned char> AuthVector(AuthString.begin(), AuthString.end());

    auto data = utility::conversions::to_base64(AuthVector);
    BasicAuthString = "Basic ";
    std::copy(data.begin(), data.end(), std::back_inserter(BasicAuthString));

    UseBasicAuth = BasicAuthUsername.length() && BasicAuthPassword.length();
}

bool CRestListener::CheckBasicAuth(web::http::http_request Request)
{
    if (!Request.headers().has(web::http::header_names::authorization))
        return false;

    for (const auto Header : Request.headers())
    {
        if (Header.first == web::http::header_names::authorization)
            return Header.second == BasicAuthString;
    }
    return false;
}

