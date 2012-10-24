//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 12th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Assert system

#ifndef _REV_CORE_CODETOOLS_ASSERT_ASSERT_H_
#define _REV_CORE_CODETOOLS_ASSERT_ASSERT_H_

#ifdef REV_ENABLE_ASSERT
#include <cassert>
#endif // REV_ENABLE_ASSERT

#include <codeTools/log/log.h>
#include <codeTools/usefulMacros.h>

namespace rev
{
#ifdef REV_ENABLE_ASSERT
	inline void revAssert(bool _condition)
	{
		assert(_condition);
	}

#else // !REV_ENABLE_ASSERT
	inline void revAssert(bool _condition)
	{
		REV_UNUSED_PARAM(_condition); // Unused parameter;
	}
#endif // !REV_ENABLE_ASSERT

	// Assert with error message
	inline void revAssert(bool _condition, const char * _errorMessage)
	{
		if( _condition == false )
		{
			(revLog() << _errorMessage << '\n').flush();
			revAssert(false);
		}
	}

}	// namespace rev

#endif // _REV_CORE_CODETOOLS_ASSERT_ASSERT_H_
