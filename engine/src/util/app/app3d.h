//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-31
//----------------------------------------------------------------------------------------------------------------------
// Base class for 3d applications
#ifndef _REV_UTIL_APP_APP3D_H_
#define _REV_UTIL_APP_APP3D_H_

#include <core/tools/profiler.h>
#ifndef ANDROID
#include <input/keyboard/keyboardInput.h>
#endif // !ANDROID
#include <engine.h>

#ifdef ANDROID
#include <android/native_activity.h>
#endif // ANDROID

#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/shader/shader.h>

namespace rev {

	class App3d {
	public:
		App3d(const core::PlatformInfo& _platformInfo);
		bool update();

	protected:
		rev::video::GraphicsDriver&	driver3d	() const { return *mDriver; }
#ifndef ANDROID
		rev::input::KeyboardInput&	keyboard	() const { return *mKeyboard; }
		rev::video::Window&			window		() const { return *mWindow; }
#endif // !ANDROID

		virtual bool				frame	(float _dt); // Usually override this
		net::http::Server*			jsonAPI	() const { return mEngine.jsonAPI(); }

	private:

	private:
		float						mMinFrameTime;
		Engine						mEngine;
		rev::video::GraphicsDriver*	mDriver = nullptr;

#ifndef ANDROID
		rev::input::KeyboardInput*	mKeyboard;
		rev::video::Window*			mWindow;
#endif // !ANDROID
		rev::core::Profiler			mProfiler;
	};

}	// namespace rev

#endif // _REV_UTIL_APP_APP3D_H_