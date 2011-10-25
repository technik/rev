////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 18th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Touch input

#include "touchInput.h"

#include "revCore/codeTools/assert/assert.h"
#ifdef _WIN32
#include "windows/touchInputWin32.h"
#endif // _WIN32
#ifdef ANDROID
#include "android/touchInputAndroid.h"
#endif // ANDROID

namespace rev { namespace input
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data definitions
	STouchInput * STouchInput::sTouchInput = 0;

	//------------------------------------------------------------------------------------------------------------------
	void STouchInput::init()
	{
		codeTools::revAssert(0 == sTouchInput);
#ifdef _WIN32
			sTouchInput = new CTouchInputWin32();
#endif // _WIN32
#ifdef ANDROID
			sTouchInput = new CTouchInputAndroid();
#endif // ANDROID
	}

	//------------------------------------------------------------------------------------------------------------------
	void STouchInput::deinit()
	{
		if(0 != sTouchInput)
			delete sTouchInput;
	}

	//------------------------------------------------------------------------------------------------------------------
	STouchInput* STouchInput::get()
	{
		return sTouchInput;
	}
}	// namespace input
}	// namespace rev
