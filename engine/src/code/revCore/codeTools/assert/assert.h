////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, code tools, assert
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// assert

#ifndef _REV_REVCORE_CODETOOLS_ASSERT_ASSERT_H_
#define _REV_REVCORE_CODETOOLS_ASSERT_ASSERT_H_

#include <revCore/codeTools/log/log.h>

namespace rev
{
#if defined(REV_ASSERT)
	inline void revAssert(bool _condition)
	{
#if defined(WIN32) || defined(_linux)
		if(_condition == false)
		{
			__asm int 3;
		}
#endif // Win32 || linux
	}

#else // !REV_ASSERT
	inline void revAssert(bool _condition)
	{
		_condition;
	}
#endif // !REV_ASSERT

	// Assert with error message
	inline void revAssert(bool _condition, const char * _errorMessage)
	{
		if( _condition == false )
		{
			revLogN(_errorMessage, eError );
			codeTools::SLog::get()->flush();
			revAssert(false);
		}
	}
}	// namespace rev

#endif // _REV_REVCORE_CODETOOLS_ASSERT_ASSERT_H_
