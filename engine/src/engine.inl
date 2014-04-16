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

#include "core/platform/osHandler.h"
#include "core/platform/platform.h"
#include "game/geometry/procedural/basic.h"
#include "math/algebra/vector.h"
#include "input/keyboard/keyboardInput.h"
#include "video/graphics/renderer/forward/forwardRenderer.h"
#include "video/graphics/renderer/renderObj.h"
#include "video/window/window.h"

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
		mRenderer = create<video::ForwardRenderer>();
		mRenderer->init(mMainWindow,*this);
		mCam = create<game::FlyByCamera>(1.54f, 0.75f, 0.01f, 1000.f);
		mCam->setSpeed(0.2f);

		mObj = game::Procedural::box(Vec3f(1.f));
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	Engine<Allocator_>::~Engine() {
		destroy(mCam);
		mRenderer->end(*this);
		destroy(mRenderer);
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

		mNode.rotate(Vec3f::zAxis(), 0.001f);
		mCam->update(0.01f);

		mRenderer->startFrame();
		mRenderer->setCamera(*mCam);
		mObj->transform = mNode.transform();
		mRenderer->renderObject(*mObj);
		mRenderer->finishFrame();
		
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