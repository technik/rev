//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "socket.h"
#include <functional>
#include <thread>

namespace rev {
	namespace net {

		class SocketServer {
		public:
			SocketServer(unsigned _port);
			~SocketServer();

			typedef std::function<void(Socket*)>	Handler;
			void setConnectionHandler(Handler _handler);

		private:
			void		startListening(addrinfo*);
			void		dispatchConnection(Socket::SocketDesc);
			void		close();

			addrinfo*	buildAddresInfo(unsigned _port);
			Socket::SocketDesc		mListener;
			bool		mIsListening = false;
			bool		mMustClose = false;
			std::thread	mListenThread;
			Handler	mConHandler;
		};
	}
}
