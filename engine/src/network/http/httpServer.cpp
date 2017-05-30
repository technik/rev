//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "httpServer.h"

#include <cassert>
#include <network/socket/socketServer.h>

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

} } }	// namespace rev::net::http