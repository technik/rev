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

			glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); // We want OpenGL 3.3
			glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); //We don't want the old OpenGL

			mNativeWindow = glfwCreateWindow((int)_size.x, (int)_size.y, _windowName, nullptr, nullptr);
			if (!mNativeWindow) {
				std::cout << "Unable to create window\n";
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		WindowWindows::~WindowWindows() {
			glfwTerminate();
		}

		//--------------------------------------------------------------------------------------------------------------
		bool WindowWindows::update() {
			glfwPollEvents();
			return !glfwWindowShouldClose(mNativeWindow);
		}
	}	// namespace video
}	// namespace rev

#endif // _WIN32