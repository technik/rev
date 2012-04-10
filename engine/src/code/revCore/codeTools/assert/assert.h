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
#ifdef REV_ASSERT
	inline void revAssert(bool _condition, const char * _errorMessage)
	{
		if( _condition == false )
		{
			revLogN(_errorMessage, eError );
			codeTools::SLog::get()->flush();
#if defined(WIN32)
			__asm int 3;
#endif // win32
		}
	}

	inline void revAssert(bool _condition)
	{
		if(_condition == false)
		{
			__asm int 3;
		}
	}
#else // ! REV_ASSERT
	inline void revAssert(bool _condition, const char * _errorMessage)
	{
		_condition;
		_errorMessage;
	}
	inline void revAssert(bool _condition)
	{
		_condition;
	}
#endif // !REV_ASSERT
}	// namespace rev

#endif // _REV_REVCORE_CODETOOLS_ASSERT_ASSERT_H_
