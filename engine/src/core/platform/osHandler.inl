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
#include <core/platform/fileSystem/fileSystem.h>
#include <core/time/time.h>
#include <input/keyboard/keyboardInput.h>

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		template<class Allocator_>
		void OSHandler::startUp(Allocator_& _alloc) {
			assert(!sInstance);
			sInstance = _alloc.template create<OSHandler>();
		}

		//--------------------------------------------------------------------------------------------------------------
		template<class Allocator_>
		void OSHandler::shutDown(Allocator_& _alloc) {
			assert(sInstance);
			_alloc.destroy(sInstance);
			sInstance = nullptr;
		}

		//--------------------------------------------------------------------------------------------------------------
		inline OSHandler* OSHandler::get() {
			assert(sInstance);
			return sInstance;
		}

		//--------------------------------------------------------------------------------------------------------------
		inline bool OSHandler::update() {
			input::KeyboardInput::get()->refresh(); // Important: refresh before BaseUpdate
			if (!OSHandlerBase::update())
				return false;
			FileSystem::get()->update();
			Time::get()->update();
			return true;
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_PLATFORM_OSHANDLER_INL_