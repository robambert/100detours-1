#include "../include/WebServer.h"
#include <iostream>
#include "../include/Utils.h"
#include "../include/Inputs.h"
#include <cpprest/http_listener.h>
#include <cpprest/json.h>

using namespace web;
using namespace web::http;
using namespace web::http::experimental::listener;

WebServer::WebServer(utility::string_t url)
	:m_listener(url)
{
	m_listener.support(methods::POST, handle_post);
}

void WebServer::handle_request(http_request request, std::function<void(json::value const&, json::value&)> action)
{
	json::value answer = json::value::object();

	request
		.extract_json()
		.then([&answer, &action](pplx::task<json::value> task)
		{
			try
			{
				json::value const& jvalue = task.get();
				display_json(jvalue, L"REQUEST: ");
				std::cout << std::endl;

				if (Inputs::is_valid_json(jvalue))
				{
					action(jvalue, answer);
				}
			}
			catch (http_exception const& e)
			{
				std::wcerr << e.what() << std::endl;
			}
		})
		.wait();

	if (!answer.has_array_field(L"appointments"))
	{
		std::cout << "STATUS CODE: BadRequest 400" << std::endl << std::endl;
		request.reply(status_codes::BadRequest, answer);
	}
	else
	{
		std::cout << "STATUS CODE: OK 200" << std::endl << std::endl;
		request.reply(status_codes::OK, answer);
	}

	display_json(answer, L"ANSWER: ");
	std::cout << std::endl;
}

void WebServer::handle_post(http_request request)
{
	std::cout << "*** Handle POST ***" << std::endl << std::endl;

	handle_request(request, [](json::value const& jvalue, json::value &answer)
	{
		Inputs inputs(jvalue);
		std::cout << "Algo running..." << std::endl << std::endl;
		run_algo(&inputs, GENETIC, answer);
	});
}

void WebServer::run()
{
	try
	{
		open().wait();

		std::cout << "Listening to request..." << std::endl << std::endl;

		while (true);
	}
	catch (std::exception const& e)
	{
		std::cerr << e.what() << std::endl;
	}
}
