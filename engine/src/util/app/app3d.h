//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-31
//----------------------------------------------------------------------------------------------------------------------
// Base class for 3d applications
#ifndef _REV_UTIL_APP_APP3D_H_
#define _REV_UTIL_APP_APP3D_H_

#include <core/memory/newAllocator.h>
#include <core/tools/profiler.h>
#include <input/keyboard/keyboardInput.h>
#include <engine.h>

namespace rev {

	namespace video {
		class GraphicsDriver;
	}

	class App3d {
	public:
		App3d();
		App3d(int _argc, const char** _argv);

		bool update();

	protected:
		rev::video::GraphicsDriver&	driver3d	() const { return *mDriver; }
		rev::input::KeyboardInput&	keyboard	() const { return *mKeyboard; }
		rev::video::Window&			window		() const { return *mWindow; }

		virtual bool				frame	(float _dt); // Usually override this
	private:
		void preFrame();
		void postFrame();

	protected:
		typedef rev::Engine<rev::core::NewAllocator>	Engine;

	private:
		Engine						mEngine;
		rev::video::GraphicsDriver*	mDriver;
		rev::video::Shader::Ptr		mShader;
		rev::input::KeyboardInput*	mKeyboard;
		rev::video::Window*			mWindow;
		rev::core::Profiler			mProfiler;
	};

}	// namespace rev

#endif // _REV_UTIL_APP_APP3D_H_