////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 22nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// time

#ifndef _REV_REVCORE_TIME_TIME_H_
#define _REV_REVCORE_TIME_TIME_H_

#include "revCore/types.h"

namespace rev
{
	class STime
	{
		/// \brief this system provides time meassures to game. time is considered constant along a whole frame.
	public:
		// --- Singleton life cycle ---
		static	void	init	();
		static	STime*	get		();	///< Returns the singleton instance
		static	void	end		();

	public:
		// --- System management ---
				void	update	();	///< Update time system

		// --- Public interface ---
		TReal frameTime	();

	private:
		STime	();

	private:
		// Singleton instance
		static STime*	sTime;

		// last frame duration.
		TReal	mFrameTime;

		// Internal use.
#if defined(_linux) || defined (ANDROID)
		int	mLastTime;
#endif
	};

	//------------------------------------------------------------------------------------------------------------------
	inline TReal STime::frameTime()
	{
		return mFrameTime;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline STime * STime::get()
	{
		return sTime;
	}

}	// namespace rev

#endif // _REV_REVCORE_TIME_TIME_H_
