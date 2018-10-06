//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#ifdef ANDROID

#include "GraphicsDriverOpenGLAndroid.h"
#include "../GraphicsDriverOpenGL.h"
#include <core/tools/log.h>

void gfxDebugCallback(GLenum source,
	GLenum type,
	GLuint id,
	GLenum severity,
	GLsizei length,
	const GLchar *_message,
	const void *userParam)
{
	rev::core::Log::error("OpenGL error");
	rev::core::Log::error(_message);
}

namespace rev::gfx {

	void printGLString(const char* name, GLenum e) {
		core::Log::info("GL ", name, " = ", glGetString(e));
	}

	GraphicsDriverGL* GraphicsDriverGLAndroid::createDriver(AndroidScreen* _screen) {
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
		ANativeWindow_setBuffersGeometry(_screen->nativeWindow, 0, 0, format);

		EGLSurface surface;
		surface = eglCreateWindowSurface(display, config, _screen->nativeWindow, NULL);

		EGLContext context;
		const EGLint ctxAttribs[] = {
			EGL_CONTEXT_CLIENT_VERSION, 3,
			EGL_NONE
		};
		context = eglCreateContext(display, config, EGL_NO_CONTEXT, ctxAttribs);

		if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
			core::Log::warning("Unable to eglMakeCurrent");
			return nullptr;
		}

		eglQuerySurface(display, surface, EGL_WIDTH, &w);
		eglQuerySurface(display, surface, EGL_HEIGHT, &h);

		auto defaultFrameBuffer = std::make_unique<DefaultFrameBuffer>(math::Vec2u(w,h));
		auto gfxDriver = new GraphicsDriverGL(std::move(defaultFrameBuffer));
		if(gfxDriver) {
			printGLString("Version", GL_VERSION);
			printGLString("Vendor", GL_VENDOR);
			printGLString("Renderer", GL_RENDERER);
			printGLString("Extensions", GL_EXTENSIONS);
			//glDebugMessageCallback(gfxDebugCallback, nullptr);
			gfxDriver->mWindow = _screen;
			gfxDriver->context = context;
			gfxDriver->surface = surface;
			gfxDriver->display = display;
			gfxDriver->mSize.x() = w;
			gfxDriver->mSize.y() = h;
			gfxDriver->mWindow->size = { unsigned(w), unsigned(h) };
			return gfxDriver;
		}

		return nullptr;
	}
		
	void GraphicsDriverGLAndroid::swapBuffers()
	{
		eglSwapBuffers(display, surface);
	}

}} // namespace rev::gfx

#endif // ANDROID