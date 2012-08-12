//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Windows openGL20 driver

#ifndef _REV_VIDEO_3DDRIVER_OPENGL20_WINDOWS_3DDRIVEROPENGL20WINDOWS_H_
#define _REV_VIDEO_3DDRIVER_OPENGL20_WINDOWS_3DDRIVEROPENGL20WINDOWS_H_

#ifdef WIN32

#include <Windows.h>

#include "../driver3dOpenGL20.h"

namespace rev { namespace video
{
	// Forward declarations
	class Window;

	class Driver3dOpenGL20Windows : public Driver3dOpenGL20
	{
	public:
		Driver3dOpenGL20Windows(const Window * window);

		void	swapBuffers	();

	private:
		HWND	mWindowHandle;
		HDC		mDrawCtxHandle;
	};

}	// namespace video
}	// namespace rev

#endif // WIN32

#endif // _REV_VIDEO_3DDRIVER_OPENGL20_WINDOWS_3DDRIVEROPENGL20WINDOWS_H_
