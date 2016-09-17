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

#include <core/time/time.h>
#include <math/algebra/vector.h>
#include <core/platform/platform.h>
#ifndef ANDROID
#include <core/platform/osHandler.h>
#include <input/keyboard/keyboardInput.h>
#include <video/window/window.h>
#endif // !ANDROID
#include <video/graphics/shader/shader.h>

using namespace rev::math;

namespace rev {

	//----------------------------------------------------------------------------------------------------------------------
#ifdef ANDROID
	Engine::Engine(ANativeActivity* _activity) {
#else // !ANDROID
	Engine::Engine(int, const char** ) {
#endif // !ANDROID
		// Create window
		core::Platform::startUp(_activity);
#ifndef ANDROID
		input::KeyboardInput::init();
		mMainWindow = new video::Window(math::Vec2u(100, 100), math::Vec2u(800, 600), "Rev window");
#endif // !ANDROID
	}

	//----------------------------------------------------------------------------------------------------------------------
	Engine::~Engine() {
#ifndef ANDROID
		input::KeyboardInput::end();
#endif //!ANDROID
		core::Platform::shutDown();
	}

	//----------------------------------------------------------------------------------------------------------------------
	bool Engine::update() {
#ifndef ANDROID
		if(!core::OSHandler::get()->update())
			return false;
#endif // !ANDROID
		
		return true;
	}

	//----------------------------------------------------------------------------------------------------------------------
	video::Window* Engine::mainWindow() const {
		assert(mMainWindow);
		return mMainWindow;
	}

}	// namespace rev

#endif // _REV_ENGINE_INL_