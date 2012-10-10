//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On October 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// 8-bit AVR Microcontroller

#ifndef _ROSE_COMPONENT_MICROCONTROLLER_AVR_AVR8BIT_H_
#define _ROSE_COMPONENT_MICROCONTROLLER_AVR_AVR8BIT_H_

#include <cstdint>

namespace rose { namespace component {
	
	class Avr8bit
	{
	public:
		Avr8bit(unsigned _flashSize);
		~Avr8bit();

		// Simulation and debugging
		bool		loadProgram		(const char* _filename);	///< Load a program into memory. Return success
		void		reset			();							///< Reset micro to power-up state.
																///< This simulates a power-off reset and doesn't erase 
																///< program memory or eeprom memory
		unsigned	setBreakPoint	(int _instruction);
		void		removeBreakPoint(unsigned _id);
		unsigned	step			();							///< Execute only one instruction. 
																///< Returns elapsed cycles.
		unsigned	run				();							///< Keep executing until a breakpoint is reached or
																///< a reset occurs
		void		showAssembly	(unsigned _start,			///< Shows assembler code for the specified region of
									unsigned _end) const;		///< code starting at _start and not including _end.
																///< If end equals 0, all occupied program memory will be
																///< displayed.
		void		showFlash		(unsigned _start,			///< Show a region of flash memory delimited by _start
									unsigned _end) const;		///< and _end in the same way as 'showAssembly'.

	private:
		bool		processHexLine	(const char * _file,		///< Processes a line of code from a program file in
									unsigned& _index);			///< Intel HEX format. Returns true on end of file.
																///< Notice _index will be incremented till the start of
																///< The next line

		uint8_t		halfByteFromHex	(const char * _digits);
		uint8_t		byteFromHex		(const char * _digits);
		uint16_t 	wordFromHex		(const char * _digits);

	private:
		uint16_t*	mFlash;		///< Flash memory
	};

}	// namespace component
}	// namespace rose
#endif // _ROSE_COMPONENT_MICROCONTROLLER_AVR_AVR8BIT_H_