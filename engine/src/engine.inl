//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Engine class
#ifndef _REV_ENGINE_INL_
#define _REV_ENGINE_INL_

#include <cassert>
#include <cstdint>

#include "engine.h"

#include <core/platform/osHandler.h>
#include <core/platform/platform.h>
#include <math/algebra/vector.h>
#include <input/keyboard/keyboardInput.h>
#include <video/window/window.h>
#include <video/graphics/shader/shader.h>

using namespace rev::math;

namespace rev {

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	Engine<Allocator_>::Engine(int _argc, const char** _argv) {
		_argc; // Unused
		_argv; // Unused

		// Create window
		core::Platform::startUp(*this);
		input::KeyboardInput::init();

		mMainWindow = create<video::Window>(math::Vec2u(100, 100), math::Vec2u(640, 480), "Rev window");
		video::Shader::Mgr::startUp(*this);
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	Engine<Allocator_>::~Engine() {
		video::Shader::Mgr::shutDown(*this);
		destroy(mMainWindow);

		input::KeyboardInput::end();
		core::Platform::shutDown(*this);
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	bool Engine<Allocator_>::update() {
		if(!core::OSHandler::get()->update())
			return false;
		
		return true;
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	video::Window* Engine<Allocator_>::mainWindow() const {
		assert(mMainWindow);
		return mMainWindow;
	}

}	// namespace rev

#endif // _REV_ENGINE_INL_