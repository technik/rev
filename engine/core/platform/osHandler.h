//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Interface with the host operating system
#ifndef _REV_CORE_PLATFORM_OSHANDLER_H_
#define _REV_CORE_PLATFORM_OSHANDLER_H_

#ifdef _WIN32
#include "windows/osHandlerWindows.h"
#endif // _WIN32
#ifdef __linux__
#include "linux/osHandlerLinux.h"
#endif // __linux__

namespace rev {
	namespace core {

		class OSHandler : public OSHandlerBase {
		public:
			// Singleton interface
			static void startUp		();
			static void shutDown	();

			static OSHandler* get();
			bool update		();
		protected:
			static OSHandler* sInstance;
		};

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_PLATFORM_OSHANDLER_H_