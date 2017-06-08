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
	namespace core {
		struct PlatformInfo;
	}
	namespace video {
		class Window;
		class ForwardRenderer;
		class RenderObj;
	} // namespace rev

	namespace net { namespace http { class Server; } }

	class Engine {
	public:
		Engine(const core::PlatformInfo& _activity);
		~Engine();

		bool update();

		video::Window* mainWindow() const;
		net::http::Server*		jsonAPI()	const { return mAPIService; }

	private:
		video::Window*			mMainWindow = nullptr;
		net::http::Server*		mAPIService = nullptr;

	private:
		// Disable copy
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;
	};

}	// namespace rev

#endif // _REV_ENGINE_H_