////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, linux video driver
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// linux video driver
#ifdef _linux

// Standard headers
#include <cstddef>		// Defines NULL
#include <GL/gl.h>		// OpenGL
#include <X11/Xlib.h>	// X11 library
// Engine headers
#include "videoDriverLinux.h"

#include "revCore/codeTools/assert/assert.h"

// Used namespaces
using namespace rev::codeTools;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	int dblBuf[]  = {GLX_RGBA, GLX_DEPTH_SIZE, 16, GLX_DOUBLEBUFFER, None};

	//------------------------------------------------------------------------------------------------------------------
	CVideoDriverLinux::CVideoDriverLinux():
		m_pDisplay(NULL),
		m_pXVisual(NULL),
		mScreenWidth(800),
		mScreenHeight(480)
	{
		createWindow();
		initOpenGL();
	}

	//------------------------------------------------------------------------------------------------------------------
	CVideoDriverLinux::~CVideoDriverLinux()
	{
		// Finish the display
		// TODO: house keeping when deleting the driver
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverLinux::endFrame()
	{
		// Swap buffers
		glXSwapBuffers(m_pDisplay, m_window);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverLinux::setScreenSize(const unsigned int _width, const unsigned int _height)
	{
		mScreenWidth = _width;
		mScreenHeight = _height;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CVideoDriverLinux::nextXEvent(XEvent &oEvent)
	{
		while(XPending(m_pDisplay)) // Loop as long as there are pending events
		{
			// Gather the next pending event
			XNextEvent(m_pDisplay, &oEvent);
			// Process the event:
			// If the event can be handled by the video driver, handle it.
			// Else return execution so it can be handled by a higher layer.
			switch(oEvent.type)
			{
			// Note: currently there is no event supported by the video driver
			default:
			{
				// The event couldn't be handled, so return execution.
				return true; // true means the returned event has not been handled.
			}
			}
		}
		// no pending events
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVideoDriverLinux::createOpenGLWindow()
	{
		// Initialise the display
		m_pDisplay = XOpenDisplay(NULL); // Open a conection to the X server
		revAssert(NULL != m_pDisplay);
		// Assert OpenGL GLX extension is supported
		int	dummy;
		revAssert(glXQueryExtension(m_pDisplay, &dummy, &dummy));
		// Find appropiate visual
		m_pXVisual = glXChooseVisual(m_pDisplay, DefaultScreen(m_pDisplay), dblBuf); // Ask for a double buffer visual
		revAssert(m_pXVisual); // Assert we got the visual
		// revAssert(TrueColor == pVisual->class); // Assert true color
		// Create a color map
		Colormap colorMap = XCreateColormap(m_pDisplay, RootWindow(m_pDisplay, m_pXVisual->screen), m_pXVisual->visual,
											AllocNone);
		XSetWindowAttributes xSWA;
		xSWA.colormap = colorMap;
		xSWA.border_pixel = 0;
		xSWA.event_mask = KeyPressMask    | ExposureMask
				 | ButtonPressMask | StructureNotifyMask;
		m_window = XCreateWindow(m_pDisplay, RootWindow(m_pDisplay, m_pXVisual->screen), 0, 0,
					  mScreenWidth, mScreenHeight, 0, m_pXVisual->depth, InputOutput, m_pXVisual->visual,
					  CWBorderPixel | CWColormap | CWEventMask, &xSWA);
		XSetStandardProperties(m_pDisplay, m_window, "main", "main", None,
						 NULL, 0, NULL);
		XMapWindow(m_pDisplay, m_window);
		
		// Create an OpenGL rendering context
		GLXContext renderContext = glXCreateContext(m_pDisplay, m_pXVisual, None, GL_TRUE);
		revAssert(NULL != renderContext);
		// Bind the rendering context and the window
		glXMakeCurrent(m_pDisplay, m_window, renderContext);
	}

}	// namespace video
}	// namespace rev

#endif // _linux
