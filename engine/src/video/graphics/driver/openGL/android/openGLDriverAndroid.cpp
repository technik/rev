//----------------------------------------------------------------------------------------------------------------------
// The MIT License (MIT)
// 
// Copyright (c) 2015 Carmelo J. Fernández-Agüera Tortosa
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Android's OpenGL driver
//----------------------------------------------------------------------------------------------------------------------
#ifdef ANDROID

#include "openGLDriverAndroid.h"
#include <video/graphics/shader/shader.h>
#include <android/native_activity.h>

#include <android/log.h>

#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "AndroidProject1.NativeActivity", __VA_ARGS__))
#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

using namespace std;

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		OpenGLDriverAndroid::OpenGLDriverAndroid()
		{
			/*Shader::manager()->setCreator(
				[](const string& _name) -> Shader* {
				string pxlName = _name + ".pxl";
				string vtxName = _name + ".vtx";
				OpenGLShader* shader = OpenGLShader::loadFromFiles(vtxName, pxlName);
				return shader;
			});
			Shader::manager()->setOnRelease([](const string& _name, Shader*) {
				string pxlName = _name + ".pxl";
				string vtxName = _name + ".vtx";
			});*/
		}

		//--------------------------------------------------------------------------------------------------------------
		void OpenGLDriverAndroid::setWindow(ANativeWindow* _window)
		{
			mWindow = _window;
			// initialize OpenGL ES and EGL

			const EGLint attribs[] = {
				EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
				EGL_BLUE_SIZE, 8,
				EGL_GREEN_SIZE, 8,
				EGL_RED_SIZE, 8,
				EGL_NONE
			};
			EGLint w, h, format;
			EGLint numConfigs;
			EGLConfig config;

			mDisplay = eglGetDisplay(EGL_DEFAULT_DISPLAY);

			eglInitialize(mDisplay, 0, 0);

			/* Here, the application chooses the configuration it desires. In this
			* sample, we have a very simplified selection process, where we pick
			* the first EGLConfig that matches our criteria */
			eglChooseConfig(mDisplay, attribs, &config, 1, &numConfigs);

			/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
			* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
			* As soon as we picked a EGLConfig, we can safely reconfigure the
			* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
			eglGetConfigAttrib(mDisplay, config, EGL_NATIVE_VISUAL_ID, &format);
			ANativeWindow_setBuffersGeometry(mWindow, 0, 0, format);

			mSurface = eglCreateWindowSurface(mDisplay, config, mWindow, NULL);
			EGLContext context;
			context = eglCreateContext(mDisplay, config, NULL, NULL);

			if (eglMakeCurrent(mDisplay, mSurface, mSurface, context) == EGL_FALSE) {
				LOGW("Unable to eglMakeCurrent");
				return;
			}

			eglQuerySurface(mDisplay, mSurface, EGL_WIDTH, &w);
			eglQuerySurface(mDisplay, mSurface, EGL_HEIGHT, &h);

			glEnable(GL_CULL_FACE);
			glEnable(GL_DEPTH_TEST);
			glDepthFunc(GL_LEQUAL);
			glLineWidth(2.f);
		}
		//--------------------------------------------------------------------------------------------------------------
		void OpenGLDriverAndroid::swapBuffers() {
			eglSwapBuffers(mDisplay, mSurface);
		}
	}
}

#endif // ANDROID