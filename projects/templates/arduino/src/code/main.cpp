// Arduino template project. Main source code
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On October 6th, 2012

// Atmel header files
#include <avr/io.h>

int main ()
{
	DDRB = 1<<7;
	PORTB = 1<<7;
	return 0;
}