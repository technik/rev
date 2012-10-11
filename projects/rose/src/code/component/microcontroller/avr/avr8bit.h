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
		Avr8bit(unsigned _flash, unsigned _ram);				///< Memory sizes are expected in kilobytes.
		~Avr8bit();

		// Simulation and debugging
		bool		loadProgram		(const char* _filename);	///< Load a program into memory. Return success
		void		reset			();							///< Reset micro to power-up state.
																///< This simulates a power-off reset and doesn't erase 
																///< program memory or eeprom memory
		unsigned	setBreakPoint	(int _instruction);
		void		removeBreakPoint(unsigned _id);
																///< a reset occurs
		void		showAssembly	(unsigned _start,			///< Shows assembler code for the specified region of
									unsigned _end) const;		///< code starting at _start and not including _end.
																///< If end equals 0, all occupied program memory will be
																///< displayed.
		void		showFlash		(unsigned _start,			///< Show a region of flash memory delimited by _start
									unsigned _end) const;		///< and _end in the same way as 'showAssembly'.

		void		simulates		(unsigned _cycles);			///< Run current program for the given cycles. This method
																///< completely ignores breakpoints and debugging aids in
																///< order to get the best performance 
		unsigned	step			();							///< Runs the program until the next instruction is reached.
																///< This usually means running just one instruction but in
																///< case of interrupt, the whole interrupt routine will be
																///< executed in the process.
																///< Returns elapsed cycles.
		unsigned	run				();							///< Keep executing until a breakpoint is reached or the
																///< program resets.
																///< Returns elapsed cycles.

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
		unsigned	mFlashSize;	///< Size of flash memory in slots (not in kilobytes).
		uint8_t*	mDataSpace;
		unsigned	mDataSize;
	};

}	// namespace component
}	// namespace rose
#endif // _ROSE_COMPONENT_MICROCONTROLLER_AVR_AVR8BIT_H_