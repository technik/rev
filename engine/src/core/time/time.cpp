//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2011/August/22
//----------------------------------------------------------------------------------------------------------------------
// Time

// Standard headers
#if defined(__linux__) || defined(ANDROID)
	#include <sys/time.h>
#elif defined (_WIN32)
	#include <Windows.h>
#endif // _linux

// Engine headers
#include "time.h"

#include <cassert>

namespace rev {
	namespace core
	{
		//------------------------------------------------------------------------------------------------------------------
		// Static data definitions
		Time* Time::sTime = 0;

		//------------------------------------------------------------------------------------------------------------------
		// Method implementations

		//------------------------------------------------------------------------------------------------------------------
		void Time::init()
		{
			assert(0 == sTime);
			sTime = new Time();
		}

		//------------------------------------------------------------------------------------------------------------------
		void Time::end()
		{
			assert(0 != sTime);
			delete sTime;
			sTime = 0;
		}

		//------------------------------------------------------------------------------------------------------------------
		void Time::update()
		{
#if defined (__linux__) || defined (ANDROID)
			// Get current time
			timeval currentTime;
			gettimeofday(&currentTime, 0);
			int usecTime = currentTime.tv_usec + 1000000 * (currentTime.tv_sec % 1000); // Not more than a thousand seconds expected
			mFrameTime = float((usecTime - mLastTime)/ 1000000.0);	// Known conversion from double. There wont be loss of
			// information because tv_usec isn't that precise.
			if(mFrameTime < 0.f)
			{
				mFrameTime += 1000.f;
			}
			mLastTime = usecTime;
#elif defined (_WIN32)
			// Get current time
			LARGE_INTEGER largeTicks;
			QueryPerformanceCounter(&largeTicks);
			unsigned currTime = largeTicks.LowPart;
			// Convert time difference to seconds
			LARGE_INTEGER frequency;
			QueryPerformanceFrequency(&frequency);
			mFrameTime = (float(currTime - mLastTime) / float(frequency.LowPart));
			// --- Force minimun frame rate, so that time wont stall while debugging
			//mFrameTime = mFrameTime < 0.1f? mFrameTime : 0.1f;
			// Replace last time
			mLastTime = currTime;
#endif // _WIN32
		}

		//------------------------------------------------------------------------------------------------------------------
		Time::Time()
			:mFrameTime(0.f)
		{
#if defined (__linux__) || defined (ANDROID)
			// Get current time
			timeval currentTime;
			gettimeofday(&currentTime, 0);
			mLastTime = currentTime.tv_usec;
#elif defined (_WIN32)
			// Get initial time
			LARGE_INTEGER largeTicks;
			QueryPerformanceCounter(&largeTicks);
			mLastTime = largeTicks.LowPart;
#endif
		}

	}	// namespace core
}	// namespace rev
