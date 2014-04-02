//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------
// Engine class
#ifndef _REV_ENGINE_H_
#define _REV_ENGINE_H_

namespace rev {

	class Engine {
	public:
		class Config {

		};

		Engine(int __argc, const char** _argv);
		~Engine();
	};

}	// namespace rev

#endif // _REV_ENGINE_H_