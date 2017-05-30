//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

namespace rev {
	namespace net {

		class SocketWin32 {
		protected:
			SocketWin32();

		private:
			static bool sWinSocketReady;
		};

		typedef SocketWin32 SocketBase;

}}	// namespace rev::net
