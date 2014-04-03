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
	}

	template <class Allocator_>
	class Engine : public Allocator_ {
	public:
		Engine(int _argc, const char** _argv);
		~Engine();

		video::Window* mainWindow() const;

	private:
		video::Window* mMainWindow = nullptr;
	};

}	// namespace rev

#include "engine.inl"

#endif // _REV_ENGINE_H_