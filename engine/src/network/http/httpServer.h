//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <functional>
#include <string>

#include <string>
#include <unordered_map>

namespace rev {
	namespace net {

		class Socket;
		class SocketServer;

		namespace http {

			class Request;
			class Response;

			class Server {
			public:
				~Server();

				void init(unsigned _port);

				/// Send response to an active connection
				void respond(unsigned _connection, const Response&);

				typedef std::function<void(Server*, unsigned _conId, const Request&)> UrlHandler;
				void setResponder(const std::string& _url, UrlHandler _responder);
				void setResponder(const std::string& _url, const http::Response&); // Sets static response for an url

			private:
				void onNewConnection(Socket* _socket);
				void sendError404(Socket* _connection) const;
				bool dispatchPetition(Server*, const std::string& _url, unsigned _conId, const Request& _petition);

			private:
				SocketServer*	mSocket;

				std::unordered_map<std::string, UrlHandler>	mHandlers;
				std::unordered_map<unsigned, Socket*>		mConnections;
			};

		}	// namespace http
} }	// namespace rev::net
