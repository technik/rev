//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 9th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Windows' video window

#ifdef WIN32

#include <Windows.h>

#include "videoWindowWindows.h"

using namespace rev::math;

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	WindowWindows::WindowWindows(const Vec2i& _pos, const Vec2u& _res)
	{
		// Assign internal values
		mPosition = _pos;
		mResolution = _res;

		// Create a windown through the windows API
		mWinapiHandle = CreateWindowA("RevWindowClass",	// Class name, registered by the video driver
		"",											// Window name (currently unsupported
		WS_CAPTION | WS_POPUP | WS_VISIBLE,			// Creation options
		_pos.x,						// X Position
		_pos.y,						// Y Position
		int(_res.x),				// Width
		int(_res.y),				// Height
		0, 0, 0, 0);				// Windows specific parameters that we don't need

		// Resize client area
		RECT rcClient;
		POINT ptDiff;
		GetClientRect(mWinapiHandle, &rcClient);
		ptDiff.x = mResolution.x - rcClient.right;
		ptDiff.y = mResolution.y - rcClient.bottom;
		MoveWindow(mWinapiHandle,mPosition.x, mPosition.y, mResolution.x + ptDiff.x, mResolution.y + ptDiff.y, TRUE);

		// Note: Maybe we could do this using SM_CYCAPTION and SM_CYBORDER instead of resizing a window.
	}

}	// namespace video
}	// namespace rev

#endif // WIN32
