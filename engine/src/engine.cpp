//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Engine class
#include <cassert>
#include <cstdint>

#include "engine.h"

#include <core/components/sceneNode.h>
#include <core/components/transformSrc.h>
#include <core/platform/osHandler.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <core/time/time.h>
#include <math/algebra/vector.h>
#include <core/platform/platform.h>
#ifndef ANDROID
#include <core/platform/osHandler.h>
#include <input/keyboard/keyboardInput.h>
#include <video/basicTypes/camera.h>
#include <video/graphics/renderObj.h>
#include <video/window/window.h>
#endif // !ANDROID
#include <video/graphics/shader/shader.h>

using namespace rev::math;

namespace rev {

	//----------------------------------------------------------------------------------------------------------------------
#ifdef ANDROID
	Engine::Engine(ANativeActivity* _activity) {
		core::Platform::startUp(_activity);
#else // !ANDROID
	Engine::Engine(int, const char** ) {
#endif // !ANDROID
#ifndef ANDROID
		core::OSHandler::startUp();
		core::Time::init();
		core::FileSystem::init();
		input::KeyboardInput::init();
		// Create window
		mMainWindow = new video::Window(math::Vec2u(100, 100), math::Vec2u(800, 600), "Rev window");
#endif // !ANDROID
		registerDataFactories();
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

	//----------------------------------------------------------------------------------------------------------------------
	void Engine::registerDataFactories() {
		// TransformSrc factories
		core::TransformSrc::registerFactory("SceneNode", core::SceneNode::construct);
		// Component factories
		core::Component::registerFactory("RenderObj", video::RenderObj::construct);
		core::Component::registerFactory("Camera", video::Camera::construct);
	}


}	// namespace rev