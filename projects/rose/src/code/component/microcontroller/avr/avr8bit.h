//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On October 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// 8-bit AVR Microcontroller

#ifndef _ROSE_COMPONENT_MICROCONTROLLER_AVR_AVR8BIT_H_
#define _ROSE_COMPONENT_MICROCONTROLLER_AVR_AVR8BIT_H_

#include <cstdint>
#include <vector>

#include "avrIOMemory.h"

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
		void		showMemory		(unsigned _start,			///< Show a region of flash memory delimited by _start
									unsigned _end) const;		///< and _end in the same way as 'showAssembly'.
		void 		showExecutionStatus() const;				///< Shows the current status of program execution.
		bool		simulate		(unsigned _cycles);			///< Run current program for the given cycles. This method
																///< completely ignores breakpoints and debugging aids in
																///< order to get the best performance.
																///< Returns true if execution succeded and false on failure. 
		unsigned	step			();							///< Runs the program until the next instruction is reached.
																///< This usually means running just one instruction but in
																///< case of interrupt, the whole interrupt routine will be
																///< executed in the process.
																///< Returns elapsed cycles.
		unsigned	run				();							///< Keep executing until a breakpoint is reached or the
																///< program resets.
																///< Returns elapsed cycles.

	private:
		// Hex file processing
		bool		processHexLine	(const char * _file,		///< Processes a line of code from a program file in
									unsigned& _index);			///< Intel HEX format. Returns true on end of file.
																///< Notice _index will be incremented till the start of
																///< The next line

		uint8_t		halfByteFromHex	(const char * _digits);
		uint8_t		byteFromHex		(const char * _digits);
		uint16_t 	wordFromHex		(const char * _digits);

		// Show debug information
		void showAssemblyInstruction(unsigned _position) const;
		void showMemoryCell			(unsigned _position) const;

		// Cache related
		void		generateExecutionTable	();
		void		createOpcodeDispatcher	();

		// Program execution
		unsigned	executeOneInstruction	();					///< Returns elapsed cycles
		void		updateTimers			(unsigned _cycles);
		uint8_t&	statusRegister			();
		uint8_t		statusRegister			() const;
		uint8_t		readSRAM				(unsigned idx) const {return mDataSpace[idx];}
		void		writeToSRAM				(unsigned idx, uint8_t _val) { mDataSpace[idx] = _val;}
		uint16_t	stackPointer			() const;
		void		setStackPointer			(uint16_t _sp);
		void		pop 					(unsigned _n, void * _dst);
		void 		push					(unsigned _n, void * _data);

	private:
		// Virtual memories
		uint16_t*		mFlash;		///< Flash memory
		unsigned		mFlashSize;	///< Size of flash memory in slots (not in kilobytes).
		uint8_t*		mDataSpace;
		unsigned		mDataSize;
		AvrIOMemory		mIOMemory;

		//Execution control
		unsigned		mProgramCounter;
		uint16_t		mCurOpcode;
		unsigned		mDelayedCycles;

		// Cache
		typedef unsigned (Avr8bit::*OpcodeDispatcher)();		///< Opcode dispatcher functions are the entry
		OpcodeDispatcher*	mExecutionTable;
		OpcodeDispatcher*	mDispatcherTable;

		// Debug
		unsigned long	mTotalSimulatedInstructions;
		std::vector<unsigned>	mCallStack;

	private:
		// Instruction set
		unsigned	unsupportedOpcode	();
		unsigned	opcode100100		();
		unsigned	opcode100101		();
		unsigned	BRBC				();
		unsigned	CALL				();
		unsigned	CPC					();
		unsigned	CPI					();
		unsigned	EOR					();
		unsigned	IN 					();
		unsigned	JMP					();
		unsigned	LDI					();
		unsigned	OUT					();
		unsigned	POP 				();
		unsigned	PUSH 				();
		unsigned	RET 				();
		unsigned	RJMP				();
	};

}	// namespace component
}	// namespace rose
#endif // _ROSE_COMPONENT_MICROCONTROLLER_AVR_AVR8BIT_H_