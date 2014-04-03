//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Interface with the host operating system
#ifndef _REV_CORE_PLATFORM_OSHANDLER_INL_
#define _REV_CORE_PLATFORM_OSHANDLER_INL_

#include "osHandler.h"

#include <cassert>

#ifdef _WIN32
#include "windows/osHandlerWindows.h"
#endif // _WIN32

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		template<class Allocator_>
		void OSHandler::startUp(Allocator_& _alloc) {
#ifdef _WIN32
			assert(sInstance);
			sInstance = _alloc.create<OSHandlerWindows<Allocator_>>();
#endif // _WIN32
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Allocator_>
		void OSHandler::shutDown(Allocator_& _alloc) {
			_alloc.destroy(sInstance);
			sInstance = nullptr;
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_PLATFORM_OSHANDLER_INL_