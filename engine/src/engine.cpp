//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
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
#include <core/platform/platformInfo.h>
#ifndef ANDROID
#include <core/platform/osHandler.h>
#include <input/keyboard/keyboardInput.h>
#include <video/basicTypes/camera.h>
#include <video/graphics/renderObj.h>
#include <video/window/window.h>
#endif // !ANDROID
#include <video/graphics/shader/shader.h>
#include <network/http/httpServer.h>

using namespace rev::math;

namespace rev {

	//----------------------------------------------------------------------------------------------------------------------
	Engine::Engine(const core::StartUpInfo& _activity) {
		core::OSHandler::startUp();
		core::Time::init();
#ifndef ANDROID
		core::FileSystem::init();
		input::KeyboardInput::init();
		// Init Json API service
		mAPIService = new net::http::Server;
		mAPIService->init(5000);
#endif
	}

	//----------------------------------------------------------------------------------------------------------------------
	Engine::~Engine() {
#ifndef ANDROID
		input::KeyboardInput::end();
		if(mNativeWindow)
			delete mNativeWindow;
#endif //!ANDROID
	}

	//----------------------------------------------------------------------------------------------------------------------
	bool Engine::update() {
#ifndef ANDROID
		input::KeyboardInput::get()->refresh(); // Important: refresh before OSHandler. Otherwise, keyboard messages may be discarded
#endif // ANDROID
		core::Time::get()->update();

#ifndef ANDROID
		if(!core::OSHandler::get()->update())
			return false;
#endif // ANDROID
		
		return true;
	}

#ifndef ANDROID
	//----------------------------------------------------------------------------------------------------------------------
	video::Window* Engine::createNativeWindow() {
		mNativeWindow = new video::Window(math::Vec2u(100, 100), math::Vec2u(800, 600), "Rev window");
		return mNativeWindow;
	}
#endif

}	// namespace rev