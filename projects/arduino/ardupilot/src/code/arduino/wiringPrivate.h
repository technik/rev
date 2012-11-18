//----------------------------------------------------------------------------------------------------------------------
// FuVe's Cierva Project.
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on October 18th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Based on arduino code

#ifndef _CIERVA_ARDUINO_WIRINGPRIVATE_H_
#define _CIERVA_ARDUINO_WIRINGPRIVATE_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include <stdarg.h>

#include <arduino/arduino.h>

#ifndef cbi
#define cbi(sfr, bit) (_SFR_BYTE(sfr) &= ~_BV(bit))
#endif
#ifndef sbi
#define sbi(sfr, bit) (_SFR_BYTE(sfr) |= _BV(bit))
#endif

#define EXTERNAL_INT_0 0
#define EXTERNAL_INT_1 1
#define EXTERNAL_INT_2 2
#define EXTERNAL_INT_3 3
#define EXTERNAL_INT_4 4
#define EXTERNAL_INT_5 5
#define EXTERNAL_INT_6 6
#define EXTERNAL_INT_7 7

#define EXTERNAL_NUM_INTERRUPTS 8

typedef void (*voidFuncPtr)(void);

#endif // _CIERVA_ARDUINO_WIRINGPRIVATE_H_