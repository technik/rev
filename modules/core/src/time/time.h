//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On October 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Time control and measurement

#ifndef _REV_CORE_TIME_TIME_H_
#define _REV_CORE_TIME_TIME_H_

namespace rev
{
	class Time
	{
		static void Init(); ///< Initialize system clock

		// Time measurement
		uint32_t	micros	();	///< Microseconds since power-up or last reset
		uint32_t	millis	(); ///< Milliseconds since power-up or last reset
		uint32_t	seconds	(); ///< Seconds since power-up or last reset

		// Delays
		void		delayMS	(uint16_t _ms);	///< Delay _ms milliseconds
		void		delayUS	(uint16_t _us); ///< Delay _us microseconds
	};
}	// namespace rev

#endif // _REV_CORE_TIME_TIME_H_
