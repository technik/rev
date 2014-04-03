//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Engine class
#ifndef _REV_ENGINE_INL_
#define _REV_ENGINE_INL_

#include "engine.h"

namespace rev {

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	Engine<Allocator_>::Engine(int _argc, const char** _argv) {
		_argc; // Unused
		_argv; // Unused
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	Engine<Allocator_>::~Engine() {
		// Intentionally blank
	}

}	// namespace rev

#endif // _REV_ENGINE_INL_