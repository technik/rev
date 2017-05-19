//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Interface with OpenGL for Windows platforms

#ifdef _WIN32

#include "openGLDriverWindows.h"
#include <video/window/window.h>
#include <core/platform/osHandler.h>
#include <string>
#include <gl/GL.h>
#include <core/platform/fileSystem/fileSystem.h>

using namespace std;
using namespace rev::core;

namespace rev {
	namespace video {
		//--------------------------------------------------------------------------------------------------------------
		OpenGLDriverWindows::OpenGLDriverWindows(Window* _window) {
			mWindowHandle = _window->winapiHandle();

			static PIXELFORMATDESCRIPTOR pfd =				// pfd Tells Windows How We Want Things To Be
			{
				sizeof(PIXELFORMATDESCRIPTOR),				// Size Of This Pixel Format Descriptor
				1,											// Version Number
				PFD_DRAW_TO_WINDOW |						// Format Must Support Window
				PFD_SUPPORT_OPENGL |						// Format Must Support OpenGL
				PFD_DOUBLEBUFFER,							// Must Support Double Buffering
				PFD_TYPE_RGBA,								// Request An RGBA Format
				16,											// Color Depth
				0, 0, 0, 0, 0, 0,							// Color Bits Ignored
				0,											// No Alpha Buffer
				0,											// Shift Bit Ignored
				0,											// No Accumulation Buffer
				0, 0, 0, 0,									// Accumulation Bits Ignored
				24,											// 16Bit Z-Buffer (Depth Buffer)  
				0,											// No Stencil Buffer
				0,											// No Auxiliary Buffer
				PFD_MAIN_PLANE,								// Main Drawing Layer
				0,											// Reserved
				0, 0, 0										// Layer Masks Ignored
			};

			// -- Some Windows-OpenGL configuration --
			mDevCtxHandle = GetDC(mWindowHandle); // Device context
			GLuint pixelFormat = ChoosePixelFormat(mDevCtxHandle, &pfd);
			SetPixelFormat(mDevCtxHandle, pixelFormat, &pfd);
			HGLRC renderCtxHandle = wglCreateContext(mDevCtxHandle);
			wglMakeCurrent(mDevCtxHandle, renderCtxHandle);

			GLenum res = glewInit();
			assert(res == GLEW_OK);
			Shader::manager()->setCreator(
				[](const string& _name) -> Shader* {
				string sourceFileName = _name + ".glsl";
				OpenGLShader* shader = OpenGLShader::loadFromFile(sourceFileName);
				if (shader) {
					auto refreshShader = [=](const string&) {
						// Recreate shader
						shader->~OpenGLShader();
						OpenGLShader::loadFromFile(sourceFileName, *shader);
					};
					FileSystem::get()->onFileChanged(sourceFileName) += refreshShader;
				}
				return shader;
			});
			Shader::manager()->setOnRelease([](const string& _name, Shader*) {
				string pxlName = _name + ".glsl";
				FileSystem::get()->onFileChanged(pxlName).clear();
			});
			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glLineWidth(2.f);
			glPointSize(4.f);

			_window->onResize() += [=]() {
				setViewport(math::Vec2i(0, 0), _window->size());
			};
		}

		//--------------------------------------------------------------------------------------------------------------
		void OpenGLDriverWindows::swapBuffers() {
			SwapBuffers(mDevCtxHandle);
		}

	}	// namespace video
}	// namespace rev

#endif // _WIN32