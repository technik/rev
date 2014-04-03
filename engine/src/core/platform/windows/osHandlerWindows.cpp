//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Interface with Windows operating system

#include "osHandlerWindows.h"

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------------------
		// Static data definition
		OSHandler* OSHandler::sInstance = nullptr;

		//--------------------------------------------------------------------------------------------------------------
		bool OSHandlerWindows::update() {
			// 666 TODO: Actually respond to windows messages
			return true;
		}

	}	// namespace core
}	// namespace rev