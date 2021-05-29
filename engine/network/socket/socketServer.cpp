//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "socketServer.h"

#include <cassert>
#include <cstring> // memset
#include <iostream>
#include <sstream>

namespace rev {
	namespace net {

		//------------------------------------------------------------------------------------------------------------------
		namespace {
			void initSocketLib() {
#ifdef _WIN32
				WSADATA wsaData;

				// Initialize Winsock
				int iResult = WSAStartup(MAKEWORD(2, 2), &wsaData);
				assert(!iResult);
#endif // _WIN32
			}
		}

		//------------------------------------------------------------------------------------------------------------------
		SocketServer::SocketServer(unsigned _port) {
			// Start listening thread
			assert(0 != _port);	// Ensure data integrity
			initSocketLib();

			addrinfo * socketAddress = buildAddresInfo(_port);
			mListener = socket(socketAddress->ai_family, socketAddress->ai_socktype, socketAddress->ai_protocol);
			//delete socketAddress;
			// assert(mListener != INVALID_SOCKET);
			if (mListener == INVALID_SOCKET) {
				std::cout << "Error: Unable to create socket server at port " << _port << "\n";
			}
			else {
				startListening(socketAddress);
			}
		}

		//------------------------------------------------------------------------------------------------------------------
		SocketServer::~SocketServer() {
			close();
		}

		//------------------------------------------------------------------------------------------------------------------
		void SocketServer::setConnectionHandler(Handler _handler) {
			mConHandler = _handler;
		}

		//------------------------------------------------------------------------------------------------------------------
		void SocketServer::startListening(addrinfo* _addr) {// Setup the TCP listening socket
			int res = ::bind(mListener, _addr->ai_addr, (int)_addr->ai_addrlen);
			if (res == SOCKET_ERROR) {
				std::cout << "Error: Unable to bind socket server\n";
				close();
			}
			res = listen(mListener, SOMAXCONN);
			if (res == SOCKET_ERROR) {
				std::cout << "Error: Unable to set socket server listening\n";
				close();
			}

			// Start listening thread
			mListenThread = std::thread([this]() {
				for (;;) {
					if (mMustClose)
						return;
					Socket::SocketDesc conn = accept(mListener, nullptr, nullptr);
					if (INVALID_SOCKET == conn) {
						closesocket(mListener);
						assert(false);
						return;
					}
					else {
						std::cout << "Accepting connection on socket " << conn << "\n";
						dispatchConnection(conn);
					}
				}
			});
			mIsListening = true;
		}

		//------------------------------------------------------------------------------------------------------------------
		void SocketServer::dispatchConnection(Socket::SocketDesc _socketDesc) {
			// 666 TODO: Do this in a separate thread in order to prevent collapsing the listening thread
			//std::thread([this,_socketDesc](){
			Socket* conn = new Socket(_socketDesc);
			mConHandler(conn);
			// });
		}

		//------------------------------------------------------------------------------------------------------------------
		void SocketServer::close() {
			assert(mListenThread.get_id() != std::this_thread::get_id()); // Ensure it's not this thread trying to delete itself
			if (mListener != INVALID_SOCKET) {
				assert(mListenThread.joinable());
				mMustClose = true;
				closesocket(mListener);
				mListenThread.join();
				mListener = INVALID_SOCKET;
			}
		}

		//------------------------------------------------------------------------------------------------------------------
		addrinfo* SocketServer::buildAddresInfo(unsigned _port) {
			// Translate port into a string
			assert(0 != _port);
			std::stringstream portName;
			portName << _port;

			// Get address information
			struct addrinfo intendedAddress;
			memset(&intendedAddress, 0, sizeof(struct addrinfo));
			intendedAddress.ai_flags = AI_PASSIVE;	// Socket address will be used to call the bind function
			intendedAddress.ai_family = AF_INET;
			intendedAddress.ai_socktype = SOCK_STREAM;
			intendedAddress.ai_protocol = IPPROTO_TCP;

			struct addrinfo *socketAddress = nullptr;
			int res = getaddrinfo(nullptr, portName.str().c_str(), &intendedAddress, &socketAddress);
			assert(res == 0);
			assert(nullptr != socketAddress);

			return socketAddress;
		}

} }	// namespace rev::net