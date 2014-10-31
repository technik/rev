//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/12
//----------------------------------------------------------------------------------------------------------------------
// Generic shader interface
#include "shader.h"

namespace rev {
	namespace core {
#ifndef _WIN32
		template<> 
#endif // _WIN32
		video::Shader::Mgr* video::Shader::Mgr::sInstance = nullptr;
	}
}
