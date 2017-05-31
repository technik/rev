//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Interface with the host operating system
#include "osHandler.h"


#include <cassert>
#include <core/platform/fileSystem/fileSystem.h>
#include <core/time/time.h>
#include <input/keyboard/keyboardInput.h>

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
			input::KeyboardInput::get()->refresh(); // Important: refresh before OSHandler. Otherwise, keyboard messages may be discarded
			if (!OSHandlerBase::update())
				return false;
			FileSystem::get()->update();
			Time::get()->update();
			return true;
		}
	}
}