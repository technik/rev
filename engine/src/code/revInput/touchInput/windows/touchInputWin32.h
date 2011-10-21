////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 18th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Touch input for windows

#ifndef _REV_REVINPUT_TOUCHINPUT_WINDOWS_TOUCHINPUTWIN32_H_
#define _REV_REVINPUT_TOUCHINPUT_WINDOWS_TOUCHINPUTWIN32_H_

// Windows headers
#include <windows.h>

// Engine headers
#include "revInput/touchInput/touchInput.h"

namespace rev { namespace input
{
	class CTouchInputWin32: public STouchInput
	{
	public:
		// Constructor & destructor
		CTouchInputWin32();
		~CTouchInputWin32();

		// Inherited interface
		unsigned	nActiveTouches	        () const;

		bool		pressed			        (unsigned _touch = 0) const;
		bool		held			        (unsigned _touch = 0) const;
		bool		released		        (unsigned _touch = 0) const;

		CVec2		touchPos		        (unsigned _touch = 0) const;

		// Platform-specific interface
	    bool        processWindowsMessage   (MSG _message);
	};
}	// namespace input
}	// namespace rev

#endif // _REV_REVINPUT_TOUCHINPUT_WINDOWS_TOUCHINPUTWIN32_H_
