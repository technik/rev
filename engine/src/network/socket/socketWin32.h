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

namespace rev {
	namespace net {

		class SocketWin32 {
		protected:
			typedef SOCKET SocketDesc;

			SocketWin32();

		private:
			static bool sWinSocketReady;
		};

		typedef SocketWin32 SocketBase;

}}	// namespace rev::net

#endif // _WIN32