//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include "pointingInput.h"

namespace rev { namespace input {

#ifdef _WIN32
	PointingInputWindows* PointingInputWindows::sInstance = 0;

	//------------------------------------------------------------------------------------------------------------------
	void PointingInputWindows::init() 
	{
		assert(!sInstance);
		sInstance = new PointingInputWindows;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool PointingInputWindows::processMessage(MSG message)
	{
		if(message.message == WM_LBUTTONDOWN)
		{
			mLeftDown = true;
			return true;
		}
		if(message.message == WM_LBUTTONUP)
		{
			mLeftDown = false;
			return true;
		}
		if(message.message == WM_MBUTTONDOWN)
		{
			mRightDown = true;
			return true;
		}
		if(message.message == WM_RBUTTONDOWN)
		{
			mRightDown = false;
			return true;
		}
		if(message.message == WM_RBUTTONUP)
		{
			mRightDown = true;
			return true;
		}
		if(message.message == WM_RBUTTONUP)
		{
			mRightDown = false;
			return true;
		}
		if(message.message == WM_MOUSEMOVE || message.message == WM_MOUSEHOVER)
		{
			auto pos = MAKEPOINTS(message.lParam);
			position.x() = pos.x;
			position.y() = pos.y;
			return true;
		}
		return false; // Nothing processed
	}
#endif // _WIN32

}}
