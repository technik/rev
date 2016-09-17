//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------
// Engine class
#ifndef _REV_ENGINE_H_
#define _REV_ENGINE_H_

#ifdef ANDROID
#include <android/native_activity.h>
#endif // ANDROID

namespace rev {
	namespace video {
		class Window;
		class ForwardRenderer;
		class RenderObj;
	} // namespace rev

	class Engine {
	public:
#ifdef ANDROID
		Engine(ANativeActivity* _activity);
#else // !ANDROID
		Engine(int _argc, const char** _argv);
#endif // !ANDROID
		~Engine();

		bool update();

		video::Window* mainWindow() const;

	private:
		video::Window*			mMainWindow = nullptr;

	private:
		// Disable copy
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;
	};

}	// namespace rev

#endif // _REV_ENGINE_H_