//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014-10-29
//----------------------------------------------------------------------------------------------------------------------
// Window for linux systems
#ifdef __linux__

#include <X11/X.h>
#include <X11/Xlib.h>
#include <GL/glew.h>

#include "windowLinux.h"

namespace rev {
	namespace video {

		WindowLinux::WindowLinux(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName)
			:mPosition(_pos)
			,mSize(_size)
			 {
			mDisplay = XOpenDisplay(NULL);
			assert(mDisplay);
			::Window parentWindow = DefaultRootWindow(mDisplay);

			GLint att[] = { GLX_RGBA, GLX_DEPTH_SIZE, 24, GLX_DOUBLEBUFFER, None };
			mVisualInfo = glXChooseVisual(mDisplay, 0, att);
			assert(mVisualInfo);

			Colormap cmap = XCreateColormap(mDisplay, parentWindow, mVisualInfo->visual, AllocNone);
			XSetWindowAttributes    swa;
			swa.colormap = cmap;
 			swa.event_mask = ExposureMask | KeyPressMask;
			mXWindow = XCreateWindow(mDisplay, parentWindow, 0, 0, 600, 600, 0,
				mVisualInfo->depth, InputOutput, mVisualInfo->visual, CWColormap | CWEventMask, &swa);
			XMapWindow(mDisplay, mXWindow);
			XStoreName(mDisplay, mXWindow, _windowName);

			// 666 TODO: Move this code to windowLinux
			GLXContext glc = glXCreateContext(mDisplay, mVisualInfo, NULL, GL_TRUE);
			glXMakeCurrent(mDisplay, mXWindow, glc);
			/*
			 while(1) {
			        XNextEvent(dpy, &xev);
			        
			        if(xev.type == Expose) {
			                XGetWindowAttributes(dpy, win, &gwa);
			                glViewport(0, 0, gwa.width, gwa.height);
			                DrawAQuad(); 
			                glXSwapBuffers(dpy, win);
			        }
			                
			        else if(xev.type == KeyPress) {
			                glXMakeCurrent(dpy, None, NULL);
			                glXDestroyContext(dpy, glc);
			                XDestroyWindow(dpy, win);
			                XCloseDisplay(dpy);
			                exit(0);
			        }
			    }
			XWindowAttributes       gwa;
			XEvent                  xev;
			*/
		}

	}	// namespace video
}	// namespace rev

#endif // __linux__