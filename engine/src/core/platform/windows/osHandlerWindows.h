//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Interface with Windows operating system
#ifndef _REV_CORE_PLATFORM_WINDOWS_OSHANDLERWINDOWS_H_
#define _REV_CORE_PLATFORM_WINDOWS_OSHANDLERWINDOWS_H_

#include "../osHandler.h"

namespace rev {
	namespace core {

		class OSHandlerWindows : public OSHandler{
		public:
			// Virtual interface
			bool update();
		};

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_PLATFORM_WINDOWS_OSHANDLERWINDOWS_H_