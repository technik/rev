//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "httpServer.h"

#include <cassert>
#include <network/socket/socketServer.h>

#include "httpRequest.h"
#include "httpResponse.h"
#include <iostream>
#include <string>

using namespace std;

namespace rev {
	namespace net {

		namespace http {

			//----------------------------------------------------------------------------------------------------------
			void Server::init(unsigned _port) {
				if(mSocket)
					delete mSocket;
				mSocket = new SocketServer(_port);
				assert(mSocket);
				std::cout << "Http server listening on port " << _port << "\n";

				// Set default responder to http requests
				mSocket->setConnectionHandler([this](Socket* _connection) {
					onNewConnection(_connection);
				});
			}

			//------------------------------------------------------------------------------------------------------------------
			Server::~Server() {
				if(mSocket)
					delete mSocket;
			}

			//------------------------------------------------------------------------------------------------------------------
			void Server::respond(unsigned _conId, const Response& _response) {
				mConnections[_conId]->write(_response.serialize());
			}

			//------------------------------------------------------------------------------------------------------------------
			void Server::setResponder(const std::string& _url, UrlHandler _handler) {
				assert(_url[0] == '/');
				mHandlers[_url] = _handler;
			}
			
			//------------------------------------------------------------------------------------------------------------------
			void Server::setResponder(const std::string& _url, const http::Response& _handler) {
				assert(_url[0] == '/');
				http::Response response = _handler;
				mHandlers[_url] = [=](Server* _srv, unsigned _conId, const http::Request&) {
					_srv->respond(_conId, response);
				};
			}

			//------------------------------------------------------------------------------------------------------------------
			void Server::onNewConnection(Socket* _connection) {
				// Generate a connection identifier
				unsigned conId = unsigned(long(_connection));
				mConnections[conId] = _connection;
				// Retrieve request
				std::string message;
				const unsigned buffSize = 2 * 1024;
				char buffer[buffSize];
				unsigned len = _connection->read(buffer, buffSize);
				if (len > 0) {
					message.append(buffer, len);
					Request petition = Request(message);

					// Locate proper responder
					if (!dispatchPetition(this, petition.url(), conId, petition))
						sendError404(_connection);
				}
				// Clean up
				_connection->close();
				mConnections.erase(mConnections.find(conId));
				delete _connection;
			}

			//------------------------------------------------------------------------------------------------------------------
			void Server::sendError404(Socket* _connection) const {
				std::string notFound = "HTTP/1.1 404 Not Found\r\n\r\nThe Url you are requesting is not available\r\n";
				_connection->write(notFound.size(), notFound.c_str());
			}

			//------------------------------------------------------------------------------------------------------------------
			bool Server::dispatchPetition(Server* _server, const std::string& _url, unsigned _conId, const Request& _petition) {
				if (_url.empty())
					return false;
				string key = _url[0] == '/' ? _url : (string("/") + _url); // Always start with a slash
				while (!key.empty()) {
					// Try key
					auto iter = mHandlers.find(key);
					if (iter != mHandlers.end()) {
						iter->second(_server, _conId, _petition); // Invoke handler
						return true;
					}
					// Not found, keep decomposing the url
					unsigned lastSlash = key.find_last_of('/');
					key = key.substr(0, lastSlash);
				}
				return false;
			}

} } }	// namespace rev::net::http