//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Singleton to interact with platform dependent systems
#ifndef _REV_CORE_PLATFORM_PLATFORM_INL_
#define _REV_CORE_PLATFORM_PLATFORM_INL_

#include "platform.h"

#include <core/memory/systemMemory.h>
#include <core/platform/osHandler.h>

namespace rev {
	namespace core {

		//----------------------------------------------------------------------------------------------------------------------
		template<typename Alloc_>
		void Platform::startUp(Alloc_& _alloc) {
			SystemMemory::startUp(_alloc);
			OSHandler::startUp(_alloc);
		}
		
		//----------------------------------------------------------------------------------------------------------------------
		template<typename Alloc_>
		void Platform::shutDown(Alloc_& _alloc) {
			OSHandler::shutDown(_alloc);
			SystemMemory::shutDown(_alloc);
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_PLATFORM_PLATFORM_INL_