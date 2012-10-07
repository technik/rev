//----------------------------------------------------------------------------------------------------------------------
// Arduino firmware for Cierva UAV.
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On October 6th, 2012
// For FuVe Cierva Project.
//----------------------------------------------------------------------------------------------------------------------
// Entry point

// Atmel header files
#include <avr/io.h>

int main ()
{
	DDRB = 1<<7;
	PORTB = 1<<7;
	return 0;
}