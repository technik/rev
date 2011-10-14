////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, code tools, assert
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// assert

#ifndef _REV_REVCORE_CODETOOLS_ASSERT_ASSERT_H_
#define _REV_REVCORE_CODETOOLS_ASSERT_ASSERT_H_

#include <assert.h>

namespace rev	{	namespace codeTools
{
	//--------------------------------------------------------------------------
	// Function prototypes
	//--------------------------------------------------------------------------
	void revAssert(bool _condition);	// halt execution if _condition fails
										// only in debug mode.

	//--------------------------------------------------------------------------
	// Implementations
	//--------------------------------------------------------------------------
#ifdef _DEBUG
	inline void revAssert(bool _condition)
	{
		assert(_condition);
	}
#else // _DEBUG
	inline void revAssert(bool /*_condition*/)
	{
		// Intentionally blank
	}
#endif // _DEBUG

}	// namespace codeTools
}	// namespace rev

#endif // _REV_REVCORE_CODETOOLS_ASSERT_ASSERT_H_
