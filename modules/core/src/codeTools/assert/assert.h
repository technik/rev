//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Assert system

#ifndef _REV_CORE_CODETOOLS_ASSERT_ASSERT_H_
#define _REV_CORE_CODETOOLS_ASSERT_ASSERT_H_

#include <codeTools/log/log.h>

namespace rev { namespace codeTools
{
#ifdef REV_ENABLE_ASSERT
	inline void assert(bool _condition)
	{
#if defined(WIN32)
		if(_condition == false)
		{
			__asm int 3;
		}
#endif // Win32
	}

#else // !REV_ENABLE_ASSERT
	inline void assert(bool _condition)
	{
		_condition; // Unused parameter;
	}
#endif // !REV_ENABLE_ASSERT

	// Assert with error message
	inline void assert(bool _condition, const char * _errorMessage)
	{
		if( _condition == false )
		{
			(revLog() << _errorMessage << '\n').flush();
			assert(false);
		}
	}

}	// namespace codeTools
}	// namespace rev

#endif // _REV_CORE_CODETOOLS_ASSERT_ASSERT_H_
