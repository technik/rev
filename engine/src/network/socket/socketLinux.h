//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef __linux__
#define WIN32_LEAN_AND_MEAN

namespace rev {
	namespace net {

		class SocketLinux {
		protected:
			typedef int SocketDesc;

			SocketLinux() {}
		};

		typedef SocketLinux SocketBase;

	}
}	// namespace rev::net

#endif // __linux__