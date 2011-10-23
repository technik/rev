////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 24th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Touch input for windows

#ifdef _WIN32

#include "touchInputWin32.h"

namespace rev { namespace input
{
	//------------------------------------------------------------------------------------------------------------------
	CTouchInputWin32::CTouchInputWin32():
		mbPressed(false),
		mbHeld(false),
		mbReleased(false),
		mMousePos(0.f,0.f)
	{}
		
	//------------------------------------------------------------------------------------------------------------------
	CTouchInputWin32::~CTouchInputWin32()
	{}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CTouchInputWin32::nMaxTouches() const
	{
		return 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CTouchInputWin32::pressed(unsigned _id) const
	{
		if(0 == _id)
			return mbPressed;
		else
			return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CTouchInputWin32::held(unsigned _id) const
	{
		if(0 == _id)
			return mbHeld;
		else
			return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CTouchInputWin32::released(unsigned _id) const
	{
		if(0 == _id)
			return mbReleased;
		else
			return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVec2 CTouchInputWin32::touchPos(unsigned _id) const
	{
		if(0 == _id)
			return mMousePos;
		else
			return CVec2::zero;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CTouchInputWin32::processWindowsMessage(MSG _message)
	{
		switch(_message.message)
		{
		case WM_LBUTTONDOWN:
			{
				POINTS point;
				point = MAKEPOINTS(_message.lParam);
				mMousePos = CVec2(point.x, point.y);
				mbPressed = true;
				return true;
			}
		case WM_LBUTTONUP:
			{
				POINTS point;
				point = MAKEPOINTS(_message.lParam);
				mMousePos = CVec2(point.x, point.y);
				mbReleased = true;
				mbHeld = false;
				// TODO:
				return true;
			}
		case WM_MOUSEMOVE:
			{
				POINTS point;
				point = MAKEPOINTS(_message.lParam);
				mMousePos = CVec2(point.x, point.y);
				// TODO:
				return true;
			}
		}
		return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CTouchInputWin32::refresh()
	{
		mbHeld = (mbHeld || mbPressed);
		mbPressed = false;
		mbReleased = false;
	}
}	// namespace input
}	// namespace rev

#endif // _WIN32
