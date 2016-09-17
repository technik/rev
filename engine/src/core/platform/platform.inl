//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Singleton to interact with platform dependent systems
#ifndef _REV_CORE_PLATFORM_PLATFORM_INL_
#define _REV_CORE_PLATFORM_PLATFORM_INL_

#include "platform.h"

#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/osHandler.h>
#include <core/time/time.h>

namespace rev {
	namespace core {

		//----------------------------------------------------------------------------------------------------------------------
#ifdef ANDROID
		inline void Platform::startUp(ANativeActivity* _activity) {
#else // !ANDROID
		inline void Platform::startUp() {
			OSHandler::startUp();
#endif // !ANDROID
			Time::init();
#ifdef ANDROID
			FileSystem::init(_activity->assetManager);
#else // !ANDROID
			FileSystem::init();
#endif // !ANDROID
		}
		
		//----------------------------------------------------------------------------------------------------------------------
		inline void Platform::shutDown() {
#ifndef ANDROID
			FileSystem::end();
#endif // !ANDROID
			Time::end();
#ifndef ANDROID
			OSHandler::shutDown();
#endif // !ANDROID
		}

	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_PLATFORM_PLATFORM_INL_