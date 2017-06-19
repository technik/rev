//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Window for Win32 systems
#ifdef _WIN32

#include <cassert>
#include <cstdlib>

#include <Windows.h>

#include "windowWindows.h"
#include <core/platform/osHandler.h>
#include <iostream>

using namespace rev::math;

namespace rev {
	namespace video {

		const size_t MAX_NAME_SIZE = 512;

		//--------------------------------------------------------------------------------------------------------------
		WindowWindows::WindowWindows(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName) 
			: mPosition(_pos)
			, mSize(_size)
		{
			if (!glfwInit()) {
				std::cout << "ERROR: could not start GLFW3\n";
			}
			mNativeWindow = glfwCreateWindow((int)_size.x, (int)_size.y, _windowName, nullptr, nullptr);
		}

		//--------------------------------------------------------------------------------------------------------------
		void WindowWindows::update() {
			glfwPollEvents();
		}
	}	// namespace video
}	// namespace rev

#endif // _WIN32