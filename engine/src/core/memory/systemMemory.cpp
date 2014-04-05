//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Manage memory at the base level

#include <cassert>
#include "systemMemory.h"

namespace rev {
	namespace core {

		//----------------------------------------------------------------------------------------------------------------------
		// Static data definition
		SystemMemory* SystemMemory::sInstance = nullptr;

	}	// namespace core
}	// namespace rev