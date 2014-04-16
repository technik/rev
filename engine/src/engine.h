//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------
// Engine class
#ifndef _REV_ENGINE_H_
#define _REV_ENGINE_H_

#include "game/scene/sceneGraph/sceneNode.h"
#include "game/scene/camera/flyByCamera.h"

namespace rev {
	namespace video {
		class Window;
		class ForwardRenderer;
		class RenderObj;
	}

	template <class Allocator_>
	class Engine : public Allocator_ {
	public:
		Engine(int _argc, const char** _argv);
		~Engine();

		bool update();

		video::Window* mainWindow() const;

	private:
		video::Window*			mMainWindow = nullptr;
		video::ForwardRenderer*	mRenderer = nullptr;
		video::RenderObj		mObj;
		game::SceneNode			mNode;
		game::FlyByCamera*		mCam;

	private:
		// Disable copy
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;
	};

}	// namespace rev

#include "engine.inl"

#endif // _REV_ENGINE_H_