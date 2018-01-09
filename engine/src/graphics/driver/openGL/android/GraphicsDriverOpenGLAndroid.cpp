//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#ifdef ANDROID

#include <android/log.h>
#include "GraphicsDriverOpenGLAndroid.h"

#define LOGW(...) ((void)__android_log_print(ANDROID_LOG_WARN, "GraphicsDriver", __VA_ARGS__))
#define LOGI(...) ((void)__android_log_print(ANDROID_LOG_INFO, "GraphicsDriver", __VA_ARGS__))
#define LOGE(...) ((void)__android_log_print(ANDROID_LOG_ERROR, "GraphicsDriver", __VA_ARGS__))

namespace rev { namespace graphics {

	void printGLString(const char* name, GLenum e) {
		const GLubyte* s = glGetString(e);
		LOGI("GL %s = %s", name, s);
	}

	GraphicsDriverGLAndroid* GraphicsDriverGLAndroid::createDriver(NativeWindow _nativeWindow) {
		// initialize OpenGL ES and EGL
		EGLDisplay display = eglGetDisplay(EGL_DEFAULT_DISPLAY);
		EGLint glVersion[2];
		eglInitialize(display, &glVersion[0], &glVersion[1]);

		/*
		* Here specify the attributes of the desired configuration.
		* Below, we select an EGLConfig with at least 8 bits per color
		* component compatible with on-screen windows
		*/
		const EGLint attribs[] = {
			EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
			EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
			EGL_BLUE_SIZE, 8,
			EGL_GREEN_SIZE, 8,
			EGL_RED_SIZE, 8,
			EGL_DEPTH_SIZE, 24,
			EGL_NONE
		};

		/* Here, the application chooses the configuration it desires. In this
		* sample, we have a very simplified selection process, where we pick
		* the first EGLConfig that matches our criteria */
		EGLint numConfigs;
		EGLConfig config;
		eglChooseConfig(display, attribs, &config, 1, &numConfigs);

		/* EGL_NATIVE_VISUAL_ID is an attribute of the EGLConfig that is
		* guaranteed to be accepted by ANativeWindow_setBuffersGeometry().
		* As soon as we picked a EGLConfig, we can safely reconfigure the
		* ANativeWindow buffers to match, using EGL_NATIVE_VISUAL_ID. */
		EGLint format, w, h;
		eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
		ANativeWindow_setBuffersGeometry(_nativeWindow, 0, 0, format);

		EGLSurface surface;
		surface = eglCreateWindowSurface(display, config, _nativeWindow, NULL);

		EGLContext context;
		const EGLint ctxAttribs[] = {
			EGL_CONTEXT_CLIENT_VERSION, 3,
			EGL_NONE
		};
		context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttribs);

		if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
			LOGW("Unable to eglMakeCurrent");
			return nullptr;
		}

		eglQuerySurface(display, surface, EGL_WIDTH, &w);
		eglQuerySurface(display, surface, EGL_HEIGHT, &h);

		auto gfxDriver = new GraphicsDriverGLAndroid();
		if(gfxDriver) {
			printGLString("Version", GL_VERSION);
			printGLString("Vendor", GL_VENDOR);
			printGLString("Renderer", GL_RENDERER);
			printGLString("Extensions", GL_EXTENSIONS);
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