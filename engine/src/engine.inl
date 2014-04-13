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
#include "math/algebra/vector.h"
#include "video/basicTypes/color.h"
#include "video/window/window.h"
#include "video/graphics/driver/openGL/openGLDriver.h"
#include "video/graphics/renderer/backend/rendererBackEnd.h"
#include "video/graphics/shader/shader.h"

using namespace rev::math;

namespace rev {

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	Engine<Allocator_>::Engine(int _argc, const char** _argv) {
		_argc; // Unused
		_argv; // Unused

		// Create window
		core::Platform::startUp(*this);
		mMainWindow = create<video::Window>(math::Vec2u(100, 100), math::Vec2u(640, 480), "Rev window");
		video::Shader::Mgr::startUp(*this);
		mGfxDriver = create<video::OpenGLDriver>(mMainWindow);
		mGfxDriver->setZCompare(true);
		mGfxDriver->setClearColor(video::Color(0.6f, 0.8f, 1.f));
		mBackEnd = create<video::RendererBackEnd>(mGfxDriver);
		mShader = video::Shader::manager()->get("test");
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	Engine<Allocator_>::~Engine() {
		destroy(mBackEnd);
		destroy(mGfxDriver);
		video::Shader::Mgr::shutDown(*this);
		destroy(mMainWindow);
		core::Platform::shutDown(*this);
	}

	//----------------------------------------------------------------------------------------------------------------------
	template<class Allocator_>
	bool Engine<Allocator_>::update() {
		if(!core::OSHandler::get()->update())
			return false;

		mGfxDriver->clearColorBuffer();
		mGfxDriver->clearZBuffer();

		math::Vec3f vtx[3];
		vtx[0] = Vec3f(0.f, 1.f, 0.f);
		vtx[1] = Vec3f(-1.f, 0.f, 0.f);
		vtx[2] = Vec3f(1.f, 0.f, 0.f);

		uint16_t indices[] = { 0, 1, 2 };

		video::RendererBackEnd::StaticGeometry geom;
		geom.indices = indices;
		geom.nIndices = 3;
		geom.nVertices = 3;
		geom.vertices = vtx;
		geom.shader = mShader;

		mBackEnd->render(geom);
		mBackEnd->flush();

		mGfxDriver->finishFrame();
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