//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <functional>
#include <string>

namespace rev {
	namespace net {

		namespace http {

			class Request;
			class Response;

			class Server {
			public:
				void init(unsigned _port);

				/// Send response to an active connection
				void respond(unsigned _connection, const Response&);

				typedef std::function<void(Server*, unsigned _conId, const Request&)> UrlHandler;
				void setResponder(const std::string& _url, UrlHandler _responder);
				void setResponder(const std::string& _url, const http::Response&); // Sets static response for an url
			};

		}	// namespace http
} }	// namespace rev::net
