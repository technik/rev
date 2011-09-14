////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, linux video driver
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// linux video driver

#ifndef _REV_REVVIDEO_DRIVER_LINUX_VIDEODRIVERLINUX_H_
#define _REV_REVVIDEO_DRIVER_LINUX_VIDEODRIVERLINUX_H_

// Standard headers
#include <GL/glx.h>		// GL-X dependency
#include <X11/X.h>		// linux graphics server

// Engine headers
#include "revVideo/videoDriver/opengl/videoDriverOpenGL.h" // openGl drivers base implementation

namespace rev{	namespace video
{
	class CColor;

	class CVideoDriverLinux : public IVideoDriverOpenGL
	{
	public:
		//--------------------------------------------------------------------------------------------------------------
		// Constructor and destructor
		//--------------------------------------------------------------------------------------------------------------
		CVideoDriverLinux	();
		~CVideoDriverLinux	();

		//--------------------------------------------------------------------------------------------------------------
		// Per frame tasks
		void	beginFrame	();
		void	endFrame	();

		//--------------------------------------------------------------------------------------------------------------
		// Accessor methods
		void	setBackgroundColor	(const CColor& _color);
		void	setScreenSize		(const unsigned int _width, const unsigned int _height);

		//--------------------------------------------------------------------------------------------------------------
		// X11 events
		bool	nextXEvent		(XEvent& oEvent);	///< If there are any pending events that can not be processed by
													///< the video system, returns true and fills oEvent with the first
													///< of them.
	private:
		//--------------------------------------------------------------------------------------------------------------
		// Initialization tasks
		void	createWindow	();					///< Creates the aplication window
		void	initOpenGL		();
		//--------------------------------------------------------------------------------------------------------------
		// Private data
		Display *		m_pDisplay;	///< X11 display interface
		Window			m_window;	///< Application window
		XVisualInfo*	m_pXVisual;	///< X visual info.

		unsigned int	mScreenWidth;
		unsigned int	mScreenHeight;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_DRIVER_LINUX_VIDEODRIVERLINUX_H_
