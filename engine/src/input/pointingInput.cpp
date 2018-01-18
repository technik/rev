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
			clickLeft = true;
			return true;
		}
		if(message.message == WM_LBUTTONUP)
		{
			clickLeft = false;
			return true;
		}
		if(message.message == WM_MOUSEMOVE)
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
