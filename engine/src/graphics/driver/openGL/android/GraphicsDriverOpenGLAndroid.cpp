//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#ifdef ANDROID

#include <android/log.h>
#include "GraphicsDriverOpenGLAndroid.h"

#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "AndroidProject1.NativeActivity", __VA_ARGS__))

namespace rev { namespace graphics {

	GraphicsDriverGLAndroid* GraphicsDriverGLAndroid::createDriver(NativeWindow _nativeWindow) {
		/*
		* Here specify the attributes of the desired configuration.
		* Below, we select an EGLConfig with at least 8 bits per color
		* component compatible with on-screen windows
		*/
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
		EGLSurface surface;
		EGLContext context;

		EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

		eglInitialize(display, 0, 0);

		/* Here, the application chooses the configuration it desires. In this
		* sample, we have a very simplified selection process, where we pick
		* the first EGLConfig that matches our criteria */
		eglChooseConfig(display, attribs, &config, 1, &numConfigs);

		/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
		* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
		* As soon as we picked a EGLConfig, we can safely reconfigure the
		* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
		eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);

		ANativeWindow_setBuffersGeometry(_nativeWindow, 0, 0, format);

		surface = eglCreateWindowSurface(display, config, _nativeWindow, NULL);
		context = eglCreateContext(display, config, NULL, NULL);

		if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
			LOGW("Unable to eglMakeCurrent");
			return nullptr;
		}

		eglQuerySurface(display, surface, EGL_WIDTH, &w);
		eglQuerySurface(display, surface, EGL_HEIGHT, &h);

		auto gfxDriver = new GraphicsDriverGLAndroid();
		if(gfxDriver) {
			gfxDriver->mWindowHandle = _nativeWindow;
			gfxDriver->context = context;
			gfxDriver->surface = surface;
			gfxDriver->display = display;
			gfxDriver->mSize.x() = w;
			gfxDriver->mSize.y() = h;
			return gfxDriver;
		}

		return nullptr;
	}
		
	void GraphicsDriverGLAndroid::swapBuffers()
	{
		eglSwapBuffers(display, surface);
	}

}} // namespace rev::graphics

#endif // ANDROID