//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Engine class
#ifndef _REV_ENGINE_INL_
#define _REV_ENGINE_INL_

#include "engine.h"

#include "video/window/window.h"

using namespace rev::video;

namespace rev {

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	Engine<Allocator_>::Engine(int _argc, const char** _argv) {
		_argc; // Unused
		_argv; // Unused

		// Create window
		Window* window = Window::createWindow(*this);
		destroy(window);
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	Engine<Allocator_>::~Engine() {
		// Intentionally blank
	}

}	// namespace rev

#endif // _REV_ENGINE_INL_