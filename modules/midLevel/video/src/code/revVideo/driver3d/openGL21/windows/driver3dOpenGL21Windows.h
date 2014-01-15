//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Windows openGL 2.1 driver

#ifndef _REV_VIDEO_3DDRIVER_OPENGL21_WINDOWS_3DDRIVEROPENGL21WINDOWS_H_
#define _REV_VIDEO_3DDRIVER_OPENGL21_WINDOWS_3DDRIVEROPENGL21WINDOWS_H_

#ifdef WIN32

#include <Windows.h>

#include "../driver3dOpenGL21.h"

namespace rev { namespace video
{
	// Forward declarations
	class Window;

	class Driver3dOpenGL21Windows : public Driver3dOpenGL21
	{
	public:
		Driver3dOpenGL21Windows(const Window * window);

		void	swapBuffers	();

	private:
		HWND	mWindowHandle;
		HDC		mDevCtxHandle;
	};

}	// namespace video
}	// namespace rev

#endif // WIN32

#endif // _REV_VIDEO_3DDRIVER_OPENGL21_WINDOWS_3DDRIVEROPENGL21WINDOWS_H_
