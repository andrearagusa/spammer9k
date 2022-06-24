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

#include "RestServer.h"
#include "CRestListener.h"
#include "CMailHandler.h"
#include "CLog.h"

bool GenericPostHandler(web::http::http_request Request, CRestListener &CurrentListener,
                        std::function<bool(web::json::value const &, std::string &)> DataHandler) {

    auto Answer = web::json::value::object();
    web::http::status_code RetCode = web::http::status_codes::OK;

    Answer[U("success")] = web::json::value::boolean(false);

    if (CurrentListener.IsBasicAuthEnabled() && !CurrentListener.CheckBasicAuth(Request))
    {
        Request.reply(web::http::status_codes::Unauthorized);
        return false;
    }

    Request.extract_json().then([&Answer, &DataHandler, &RetCode](pplx::task<web::json::value> task)
    {
        try
        {
            auto const &JsonRequest = task.get();

            if (JsonRequest.is_null()) {
                Answer[U("error")] = web::json::value::string(U("Empty request"));
                RetCode = web::http::status_codes::BadRequest;
                return;
            }

            std::string ErrorStr;
            if (!DataHandler(JsonRequest, ErrorStr)) {
                Answer[U("error")] = web::json::value::string(U(ErrorStr));
                RetCode = web::http::status_codes::UnprocessableEntity;
                return;
            }

            Answer[U("success")] = web::json::value::boolean(true);
            Answer[U("error")] = web::json::value::null();
            RetCode = web::http::status_codes::OK;
        }

        catch (web::http::http_exception const &ex)
        {
            Answer[U("error")] = web::json::value::string(U(ex.what()));
            LOG_DEBUG() << "GetMailPostCB: DataHandler: " << ex.what() << std::endl;
            RetCode = web::http::status_codes::InternalError;
        }
    })
    .wait();

    Request.reply(RetCode, Answer);
    return true;
}

tHandlerCallback GetMailPostCB(CRestListener &CurrentListener, const std::shared_ptr<CMailHandler> &CurrentHandler)
{
    static auto &CurHandler = CurrentHandler;

    auto DataHandler = [&](web::json::value const &Data, std::string &Error) -> bool
    {
        for (auto Mail : Data.as_array()) {
            std::string Address, Subject, Content, Priority, Current;

            //auto DataObj = Data.as_object();
            try {
                Current = "address";
                Address = Mail.at(Current).as_string();

                Current = "subject";
                Subject = Mail.at(Current).as_string();

                Current = "content";
                Content = Mail.at(Current).as_string();

                Current = "priority";
                Priority = Mail.at(Current).as_string();
            }
            catch (const web::json::json_exception &e) {
                Error = "Error while parsing JSON field: " + Current + " (" + e.what() + ")";
                LOG_DEBUG() << "GetMailPostCB: DataHandler: " << Error << std::endl;
                return false;
            }
            CurHandler->PushMail(Address, Subject, Content, Priority);
        }
        return true;
    };

    return [&CurrentListener, &DataHandler](web::http::http_request Request) {
        GenericPostHandler(Request, CurrentListener, DataHandler);
    };
}

tHandlerCallback GetNewsletterPostCB(CRestListener &CurrentListener, const std::shared_ptr<CMailHandler> &CurrentHandler, CTemplateManager &TemplateManager)
{
    static auto &TempMan = TemplateManager;
    static auto &CurHandler = CurrentHandler;

    auto DataHandler = [&](web::json::value const &Data, std::string &Error) -> bool
    {
        for (auto Newsletter : Data.as_array())
        {
            //auto DataObj = Data.as_object();
            std::string Current, Template, Name, Address;
            try
            {
                Current = "template";
                Template = Newsletter.at(Current).as_string();

                Current = "username";
                Name = Newsletter.at(Current).as_string();

                Current = "email";
                Address = Newsletter.at(Current).as_string();
            }
            catch (const web::json::json_exception &e)
            {
                Error = "Error while parsing JSON field: " + Current + " (" + e.what() + ")";
                LOG_DEBUG() << "GetMailPostCB: DataHandler: " << Error << std::endl;
                //return false;
            }

            //if ( Template != DataObj.end())
            //{
            if (!TempMan.CheckTemplate(Template))
            {
                Error = "The template " + Template + " does not exist";
                LOG_DEBUG() << "GetMailPostCB: DataHandler: " << Error << std::endl;
                //return false;
            }

            auto MailBody = TempMan.GetTemplate(Template).Render(Newsletter.serialize());
            CurHandler->PushMail(Address, Name + ", Your daily newsletter", MailBody);

            //}
            //else
            //{
            //    Error = "You must specify which template to use";
            //    LOG_DEBUG() << "GetMailPostCB: DataHandler: " << Error << std::endl;
            //    return false;
            //}
        }
        return true;
    };

    return [&CurrentListener, &DataHandler](web::http::http_request Request) {
        GenericPostHandler(Request, CurrentListener, DataHandler);
    };
}

tHandlerCallback PingGetCB(CRestListener &CurrentListener)
{
    return [&CurrentListener](web::http::http_request Request) {
        //auto Answer = web::json::value::object();
        web::http::status_code RetCode = web::http::status_codes::OK;

        if (CurrentListener.IsBasicAuthEnabled() && !CurrentListener.CheckBasicAuth(Request))
        {
            Request.reply(web::http::status_codes::Unauthorized);
            return false;
        }

        //Request.extract_json().then([](pplx::task<web::json::value> task) {} ).wait();

        Request.reply(RetCode);
        return true;
    };
}
