//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2011/August/22
//----------------------------------------------------------------------------------------------------------------------
// Time
#include <cassert>

#ifndef _REV_CORE_TIME_TIME_H_
#define _REV_CORE_TIME_TIME_H_

namespace rev
{
	namespace core {
		class Time
		{
			/// \brief this system provides time meassures to game. time is considered constant along a whole frame.
		public:
			// --- Singleton life cycle ---
			static	void	init();
			static	Time*	get();	///< Returns the singleton instance
			static	void	end();

		public:
			// --- System management ---
			void	update();	///< Update time system

			// --- Public interface ---
			float frameTime();	// in seconds
			float timeMark();

		private:
			Time();

		private:
			// Singleton instance
			static Time*	sTime;

			// last frame duration.
			float	mFrameTime;

			// Internal use.
#if defined(__linux__) || defined (ANDROID)
			int	mLastTime;
#endif
#if defined (_WIN32)
			unsigned mLastTime;
#endif
		};

		//------------------------------------------------------------------------------------------------------------------
		inline float Time::frameTime()
		{
			return mFrameTime;
		}

		//------------------------------------------------------------------------------------------------------------------
		inline Time * Time::get()
		{
			assert(sTime);
			return sTime;
		}
	}	// namespace core
}	// namespace rev

#endif // _REV_CORE_TIME_TIME_H_