//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On October 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Time control and measurement

#ifndef _REV_CORE_TIME_TIME_H_
#define _REV_CORE_TIME_TIME_H_

#ifdef ATMEGA
#include <stdio.h>
#else // !ATMEGA
#include <cstdint>
#endif // !ATMEGA

namespace rev
{
	class Time
	{
		static void		init	(); ///< Initialize system clock

		// Time measurement
		static uint32_t	micros	();	///< Microseconds since power-up or last reset
		static uint32_t	millis	(); ///< Milliseconds since power-up or last reset
		static uint32_t	seconds	(); ///< Seconds since power-up or last reset

		// Delays
		static void		delayMS	(uint16_t _ms);	///< Delay _ms milliseconds
		static void		delayUS	(uint16_t _us); ///< Delay _us microseconds
	};
}	// namespace rev

#endif // _REV_CORE_TIME_TIME_H_
