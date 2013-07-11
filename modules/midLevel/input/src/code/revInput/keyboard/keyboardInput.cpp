////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 13th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Keyboard input

#include "keyboardInput.h"
#ifdef WIN32
#include "windows/keyboardInputWindows.h"
#endif // WIN32

#include <revCore/codeTools/assert/assert.h>

namespace rev { namespace input
{
	//------------------------------------------------------------------------------------------------------------------
	KeyboardInput * KeyboardInput::sInstance = 0;

	//------------------------------------------------------------------------------------------------------------------
	void KeyboardInput::init()
	{
		revAssert(0 == sInstance);
#ifdef WIN32
		sInstance = new KeyboardInputWindows();
#endif // WIN32
		revAssert(0 != sInstance);
	}


	//------------------------------------------------------------------------------------------------------------------
	void KeyboardInput::end()
	{
		revAssert(0 != sInstance);
		delete sInstance;
		sInstance = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	KeyboardInput * KeyboardInput::get()
	{
		return sInstance;
	}
}	// namespace input
}	// namespace rev