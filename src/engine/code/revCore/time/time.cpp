////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 22nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// time

// Standard headers
#if defined(_linux) || defined(ANDROID)
#include <sys/time.h>
#endif // _linux

// Engine headers
#include "time.h"

#include "revCore/codeTools/assert/assert.h"

// Used namespaces
using namespace rev::codeTools;

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	// Static data definitions
	STime* STime::sTime = 0;

	//------------------------------------------------------------------------------------------------------------------
	// Method implementations

	//------------------------------------------------------------------------------------------------------------------
	void STime::init()
	{
		revAssert(0 == sTime);
		sTime = new STime();
	}

	//------------------------------------------------------------------------------------------------------------------
	void STime::end()
	{
		revAssert(0 != sTime);
		delete sTime;
		sTime = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	void STime::update()
	{
	#if defined (_linux) || defined (ANDROID)
		// Get current time
		timeval currentTime;
		gettimeofday(&currentTime, 0);
		int usecTime = currentTime.tv_usec;
		mFrameTime = TReal((usecTime - mLastTime)/ 1000000.0);	// Known conversion from double. There wont be loss of
																// information because tv_usec isn't that precise.
		if(mFrameTime < 0.f)
		{
			mFrameTime += 1.f;
		}
		mLastTime = usecTime;
	#else
		revAssert(false); // Unimplemented functionality!!!
	#endif // _linux
	}

	//------------------------------------------------------------------------------------------------------------------
	STime::STime():
		mFrameTime(0.f)
	{
	#if defined (_linux) || defined (ANDROID)
		// Get current time
		timeval currentTime;
		gettimeofday(&currentTime, 0);
		mLastTime = currentTime.tv_usec;
	#endif
	}

}	// namespace rev
