//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Interface with the host operating system
#include "osHandler.h"


#include <cassert>
#include <core/platform/fileSystem/fileSystem.h>

namespace rev {
	namespace core {
		//--------------------------------------------------------------------------------------------------------------
		// Static data definition
		OSHandler* OSHandler::sInstance = nullptr;

		//--------------------------------------------------------------------------------------------------------------
		void OSHandler::startUp() {
			assert(!sInstance);
			sInstance = new OSHandler();
		}

		//--------------------------------------------------------------------------------------------------------------
		void OSHandler::shutDown() {
			assert(sInstance);
			delete sInstance;
			sInstance = nullptr;
		}

		//--------------------------------------------------------------------------------------------------------------
		OSHandler* OSHandler::get() {
			assert(sInstance);
			return sInstance;
		}

		//--------------------------------------------------------------------------------------------------------------
		bool OSHandler::update() {
			if (!OSHandlerBase::update())
				return false;
#ifndef ANDROID
			FileSystem::get()->update();
#endif // ANDROID
			return true;
		}
	}
}