#ifndef WEBSERVER_H
#define WEBSERVER_H

#include <iostream>
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

class WebServer
{
    public:

        WebServer(utility::string_t url);

		pplx::task<void> open() { return m_listener.open(); }
		pplx::task<void> close() { return m_listener.close(); }

		static void handle_request(web::http::http_request request, std::function<void(web::json::value const&, web::json::value&)> action);
		static void handle_post(web::http::http_request request);

		void run();


    private:

        web::http::experimental::listener::http_listener m_listener;
};

#endif // WEBSERVER_H
