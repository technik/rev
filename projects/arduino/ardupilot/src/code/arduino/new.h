//----------------------------------------------------------------------------------------------------------------------
// FuVe's Cierva Project.
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on October 18th, 2012
//----------------------------------------------------------------------------------------------------------------------
/* Header to define new/delete operators as they aren't provided by avr-gcc by default
   Taken from http://www.avrfreaks.net/index.php?name=PNphpBB2&file=viewtopic&t=59453 
 */

#ifndef _CIERVA_ARDUINO_NEW_H_
#define _CIERVA_ARDUINO_NEW_H_

#include <stdlib.h>

void * operator new(size_t size);
void operator delete(void * ptr); 

__extension__ typedef int __guard __attribute__((mode (__DI__)));

extern "C" int __cxa_guard_acquire(__guard *);
extern "C" void __cxa_guard_release (__guard *);
extern "C" void __cxa_guard_abort (__guard *); 

extern "C" void __cxa_pure_virtual(void);

#endif // _CIERVA_ARDUINO_NEW_H_
