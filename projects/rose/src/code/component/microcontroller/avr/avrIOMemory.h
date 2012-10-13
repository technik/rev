//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On April 22nd, 2012
//-------------------------------------------------------------------------------------------------------------------
// AVR Archictecture, standard IO Memory module

#ifndef _ROSE_COMPONENT_MICROCONTROLLER_AVR_AVRIOMEMORY_H_
#define _ROSE_COMPONENT_MICROCONTROLLER_AVR_AVRIOMEMORY_H_

#include <cstdint>

namespace rose { namespace component {

class AvrIOMemory
{
public:
	AvrIOMemory(){}
	AvrIOMemory(uint8_t * _memoryStart);

	// Read the value of some register
	unsigned read(unsigned _register) const;

	// -- CPU interface --
	void write(unsigned _register, uint8_t _value); // Writing to the registers can trigger behavior changes
													 // to some modules
	// -- Modules interface --
	// Manually set a register to the given value. This interface must not be used by CPU instructions
	void setRegister(unsigned _register, unsigned _value);

	// Reset
	void resetMemory();
private:
	uint8_t * mRegisters;
};

}	// namespace component
}	// namespace rose

#endif // _ROSE_COMPONENT_MICROCONTROLLER_AVR_AVRIOMEMORY_H_