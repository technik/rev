//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "socketWin32.h"
#endif // _WIN32

#ifdef __linux__
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#include "socketLinux.h"

const int INVALID_SOCKET = -1;
const int SOCKET_ERROR = -1;

#define closesocket( S ) ::close( S )
#endif // __linux__

#include <mutex>
#include <string>
#include <thread>

namespace rev {
	namespace net {

		class Socket : private SocketBase {
		public:
#ifdef __linux__
			typedef int	SocketDesc;
#endif // __linux__
#ifdef _WIN32
			typedef SOCKET SocketDesc;
#endif // _WIN32
			enum class Protocol {
				TCP,
				UDP
			};

			Socket(SocketDesc _desc = INVALID_SOCKET);

			~Socket();

			bool open(const std::string& _url, unsigned _port, Protocol = Protocol::TCP);
			void close();

			bool isOpen() const;

			bool			write(unsigned _length, const void* _data);
			bool			write(const std::string&); // Sugar for writing strings
													   // Returns the number of bytes actually read
			int		read(void* _dstbuffer, unsigned _maxLen);

		private:

			bool getSocketAddress(const std::string& _url, unsigned _port, Protocol _protocol);
			bool openSocket();
			bool connectionLess() const;

			SocketDesc mSocket = INVALID_SOCKET;
			bool mMustClose = false;
			struct addrinfo* mAddress;
			struct addrinfo* mSystemAddress;
			std::string mInBuffer;

			Protocol		mProtocol;
		};
	}	// namespace net
}	// namespace rev
