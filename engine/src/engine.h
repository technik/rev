//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------
// Engine class
#ifndef _REV_ENGINE_H_
#define _REV_ENGINE_H_

namespace rev {
	namespace video {
		class Window;
		class GraphicsDriver;
	}

	template <class Allocator_>
	class Engine : public Allocator_ {
	public:
		Engine(int _argc, const char** _argv);
		~Engine();

		bool update();

		video::Window* mainWindow() const;

	private:
		video::Window* mMainWindow = nullptr;
		// Disable copy
		Engine(const Engine&) = delete;
		Engine(Engine&&) = delete;
		Engine& operator=(const Engine&) = delete;
		Engine& operator=(Engine&&) = delete;
	};

}	// namespace rev

#include "engine.inl"

#endif // _REV_ENGINE_H_