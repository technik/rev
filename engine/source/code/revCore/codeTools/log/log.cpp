////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, code tools, log
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// log

#include <cstddef>

#include "log.h"

#include "revCore/codeTools/assert/assert.h"

namespace rev	{	namespace codeTools
{
	//--------------------------------------------------------------------------
	// Static data definitions
	//--------------------------------------------------------------------------
	SLog * SLog::s_pLog = NULL;

	//--------------------------------------------------------------------------
	// Function implementations
	//--------------------------------------------------------------------------

	//--------------------------------------------------------------------------
	void SLog::init()
	{
		// Assert the singleton isn't instantiated yet.
		revAssert(NULL == s_pLog);
		// Create the singleton instance
		s_pLog = new SLog();
	}

	//--------------------------------------------------------------------------
	void SLog::end()
	{
		// Assert there is an instance to delete
		revAssert(NULL != s_pLog);
		// Delete the instance
		delete s_pLog;
		// Restore the pointer
		s_pLog = NULL;
	}

	//--------------------------------------------------------------------------
	SLog::SLog()
	{
	}

	//--------------------------------------------------------------------------
	SLog::~SLog()
	{
	}

}	// namespace codeTools
}	// namespace rev
