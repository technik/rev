//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 9th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Windows video driver

#ifdef WIN32

#include <Windows.h>

#include <types/window/windows/videoWindowWindows.h>
#include "videoDriverWindows.h"

using namespace rev::math;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	VideoDriverWindows::VideoDriverWindows()
		:mIsWindowClassRegistered(false)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	Window * VideoDriverWindows::createWindow(const Vec2i& _pos, const Vec2u& _res)
	{
		if(!mIsWindowClassRegistered)
			registerWindowClass();
		return new WindowWindows(_pos, _res);
	}

	//------------------------------------------------------------------------------------------------------------------
	void VideoDriverWindows::destroyWindow(Window * _window)
	{
		delete _window;
	}

	//------------------------------------------------------------------------------------------------------------------
	void VideoDriverWindows::registerWindowClass()
	{
		HINSTANCE moduleHandle = GetModuleHandle(NULL);
		// -- Register a new window class --
		WNDCLASSA winClass = {
			CS_HREDRAW | CS_VREDRAW | CS_OWNDC, // Class style
			DefWindowProc,
			0,
			0,
			moduleHandle,
			NULL,	// Default icon
			LoadCursor(NULL, IDC_ARROW),	// No cursor shape
			NULL,
			NULL,
			"RevWindowClass"};

		RegisterClassA(&winClass);
	}

	//------------------------------------------------------------------------------------------------------------------
	Vec2u VideoDriverWindows::screenResolution() const
	{
		int x = GetSystemMetrics(SM_CXFULLSCREEN);
		int y = GetSystemMetrics(SM_CYFULLSCREEN);
		return Vec2u(unsigned(x), unsigned(y));
	}

}	// namespace video
}	// namespace rev

#endif // WIN32
