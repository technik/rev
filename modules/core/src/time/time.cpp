//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On October 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Time control and measurement

#include "time.h"

#ifdef ATMEGA
#include <avr/interrupt.h>
#include <avr/io.h>
// Notice this must be outside the namespace for the interrupt to work
//----------------------------------------------------------------------------------------------------------------------
// Global data
//----------------------------------------------------------------------------------------------------------------------
volatile uint16_t gTicksH = 0; // High word for tick count

//----------------------------------------------------------------------------------------------------------------------
// Interrupt handler
//----------------------------------------------------------------------------------------------------------------------
ISR(TIMER1_OVF_vect)
{
	++gTicksH;
}

#endif // ATMEGA

namespace rev
{
#ifdef ATMEGA
	//------------------------------------------------------------------------------------------------------------------
	void Time::init()
	{
		TCCR1B |= 1 << 1; // Clock source equals system clock / 8, 0.5 uS per tick
		TIMSK1 |= 1 << 0; // Enable overflow interrupt
	}

	//------------------------------------------------------------------------------------------------------------------
	uint32_t Time::micros()
	{
		return (((uint32_t)gTicksH) << 15) | (TCNT1 >> 1); // Returns nTicks / 2 (one tick is half a second)
	}

	//------------------------------------------------------------------------------------------------------------------
	uint32_t Time::millis()
	{
		// TODO: This function could use a more direct approach over gTicksH not to lose so much information.
		// Maybe gTicksH should get 32 bits for that.
		return micros() / 1000;
	}

	//------------------------------------------------------------------------------------------------------------------
	uint32_t Time::seconds()
	{
		// TODO: This function could use a more direct approach over gTicksH not to lose so much information.
		// Maybe gTicksH should get 32 bits for that.
		return micros() / 1000000;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Time::delayMS(uint16_t _ms)
	{
		uint32_t startMilliseconds = millis();
		while((millis() - startMilliseconds) < _ms)
		{}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Time::delayUS(uint16_t _us)
	{
		uint32_t startMicroseconds = micros();
		while((micros() - startMicroseconds) < _us)
		{}
	}
	
#endif // ATMEGA
}	// namespace rev
