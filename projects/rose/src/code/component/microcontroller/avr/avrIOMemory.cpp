//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On April 22nd, 2012
//-------------------------------------------------------------------------------------------------------------------
// AVR Archictecture, standard IO Memory module

#include "avrIOMemory.h"

namespace rose { namespace component {
	//-------------------------------------------------------------------------------------------------------------------
	AvrIOMemory::AvrIOMemory(uint8_t * _memoryStart)
		:mRegisters(&_memoryStart[0x20]) // Data space's address 0x20 is the start of the IO memory
	{
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned AvrIOMemory::read(unsigned _idx) const
	{
		return mRegisters[_idx];
	}

	//-------------------------------------------------------------------------------------------------------------------
	void AvrIOMemory::write(unsigned _idx, uint8_t _val)
	{
		mRegisters[_idx] = _val;
	}

	//-------------------------------------------------------------------------------------------------------------------
	void AvrIOMemory::resetMemory()
	{
		for(unsigned i = 0; i < 0x3f; ++i)
			mRegisters[i] = 0;
		// Point the stack pointer to the top address of DataSpace
		mRegisters[0x3e] = 0x21; // SP High
		mRegisters[0x3d] = 0xff; // SP Low
	}
}	// namespace component
}	// namespace rose