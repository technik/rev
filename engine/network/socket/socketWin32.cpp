//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "socketWin32.h"

#ifdef _WIN32

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>

#include "socketWin32.h"

#include <iostream>

namespace rev {
	namespace net {

		//------------------------------------------------------------------------------------------------------------------
		// Static data definitions
		//------------------------------------------------------------------------------------------------------------------
		bool SocketWin32::sWinSocketReady = false;

		//------------------------------------------------------------------------------------------------------------------
		SocketWin32::SocketWin32() {
			if (!sWinSocketReady) {
				WSADATA wsData;
				int res = WSAStartup(MAKEWORD(2, 2), &wsData);
				if (res == 0) // Success
					sWinSocketReady = true;
				else
					std::cout << "Error: Couldn't init winSocket library. Returned error " << res << "\n";
			}
		}

} }	// namespace rev::net

#endif // _WIN32