//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On October 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// 8-bit AVR Microcontroller

#include "avr8bit.h"

#include <cstdint>
#include <iostream>

#include <codeTools/assert/assert.h>
#include <codeTools/log/log.h>
#include <file/file.h>
using rev::File;
using rev::revLog;

namespace rose { namespace component {

	//------------------------------------------------------------------------------------------------------------------
	Avr8bit::Avr8bit(unsigned _flash, unsigned _ram)
		:mFlashSize(_flash * 512) // slots = _kilobytes * 1024 / 2.
		,mDataSize(_ram * 1024 + 0x200) // Data memory + register file + i/o memory + extended i/o memory.
		,mProgramCounter(0)
		,mCurOpcode(0)
		,mDelayedCycles(0)
		,mDispatcherTable(nullptr)
		,mTotalSimulatedInstructions(0)
	{
		// Allocate program memory and execution cache
		mFlash = new uint16_t[mFlashSize];
		mExecutionTable = new OpcodeDispatcher[mFlashSize];

		// Allocate data memories
		mDataSpace = new uint8_t[mDataSize];
		new(&mIOMemory) AvrIOMemory(mDataSpace);
	}

	//------------------------------------------------------------------------------------------------------------------
	Avr8bit::~Avr8bit()
	{
		delete[] mFlash;
		delete[] mDataSpace;
		delete[] mExecutionTable;
		if(nullptr != mDispatcherTable)
			delete[] mDispatcherTable;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Avr8bit::loadProgram(const char* _filename)
	{
		// First of all, ensure incoming data is valid
		if((nullptr == _filename) || (*_filename == '\0'))
			return false; // Invalid filename

		// Now open the file
		File * hexFile = File::open(_filename);
		if(!hexFile)
			return false;
		// Process the file and load contents to flash memory
		const char * hexCode = hexFile->bufferAsText();
		bool reachedEndOfFile = false;
		unsigned hexLineIndex = 0;
		while(! reachedEndOfFile)
		{
			reachedEndOfFile = processHexLine(hexCode, hexLineIndex);
		}
		delete hexFile;

		// Once we have the program loaded into the flash memory, we can compute the cached execution table
		// to speed up the emulation
		generateExecutionTable();
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::reset()
	{
		for(unsigned i = 0; i < mDataSize; ++i)
			mDataSpace[i] = 0x00;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::showAssembly(unsigned _start, unsigned _end) const
	{
		revLog() << "Assembly\n" << "Address\t\tOpcode\tName\n";
		if(_start >= mFlashSize || _start >= _end)
			return; // There is no code after _start
		if(_end > mFlashSize)
			_end = mFlashSize; // Clamp section
		for(unsigned i = _start; i < _end; ++i)
		{
			showAssemblyInstruction(i);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::showMemory(unsigned _start, unsigned _end) const
	{
		std::cout << "Memory\n" << "Address\tValue\n";
		if(_start >= mDataSize || _start >= _end)
			return; // There is no code after _start
		if(_end > mFlashSize)
			_end = mFlashSize; // Clamp section
		for(unsigned i = _start; i < _end; ++i)
		{
			showMemoryCell(i);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::showExecutionStatus() const
	{
		std::cout << "Program Counter = " << mProgramCounter << std::endl
				  << "Simulated instructions = " << mTotalSimulatedInstructions << std::endl;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Avr8bit::simulate(unsigned _cycles)
	{
		if(_cycles > mDelayedCycles)
		{
			// Clear delayed cycles
			_cycles -= mDelayedCycles;
			mDelayedCycles = 0;
			// Actually run some instructions
			while(_cycles > 0)
			{
				// Execute just one instruction
				unsigned cyclesElapsedByInstruction = executeOneInstruction();
				if(0 == cyclesElapsedByInstruction)
					return false; // Failure on execution.
				// Update cycle count
				if(cyclesElapsedByInstruction > _cycles)
					_cycles = 0;
				else _cycles -= cyclesElapsedByInstruction;
				// Update internal timers of the microcontroller
				updateTimers(cyclesElapsedByInstruction);
			}
		}
		else mDelayedCycles -= _cycles; // Not enough cycles to run anything
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Avr8bit::processHexLine(const char* _buffer, unsigned& _idx)
	{
		// _idx must be the offset inside the _buffer where the target line starts at.
		rev::revAssert(_buffer[_idx] == ':');
		uint8_t registryLen = byteFromHex(&_buffer[_idx+1]);
		uint16_t address = wordFromHex(&_buffer[_idx+3]);
		uint8_t dataType = byteFromHex(&_buffer[_idx+7]);
		if(0x01 == dataType) // End of file reached
			return true;
		// If we didn't reach End of file, read data
		if(0x00 == dataType) // Simple data
		{
			char * memBuffer = reinterpret_cast<char*>(mFlash);
			for(unsigned i = 0; i < registryLen; ++i) // Copy data info flash memory
				memBuffer[address + i] = byteFromHex(&_buffer[_idx+9+2*i]);
		}
		else
		{
			rev::revAssert(false, "Unsupported data type in Hex file");
		}
		// Move index to the start of the next line:
		// 1b colon + 2b regLen + 4b address + 2b dataType + 2b checksum  + 2b (new line+carr ret)= 13b per line
		_idx += 2*registryLen + 13;
		return false; // Didn't reach End of File.
	}

	//------------------------------------------------------------------------------------------------------------------
	uint8_t Avr8bit::halfByteFromHex(const char * _digits)
	{
		if(*_digits < 'A')	// Decimal number
			return *_digits - '0';
		else				// Letter
			return *_digits - 'A' + 0xA;
	}

	//------------------------------------------------------------------------------------------------------------------
	uint8_t Avr8bit::byteFromHex(const char * _digits)
	{
		return (halfByteFromHex(_digits) << 4) | halfByteFromHex(&_digits[1]);
	}

	//------------------------------------------------------------------------------------------------------------------
	uint16_t Avr8bit::wordFromHex(const char * _digits)
	{
		return (byteFromHex(_digits) << 8) | byteFromHex(&_digits[2]);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::showAssemblyInstruction(unsigned _position) const
	{
		// Display opcode
		uint16_t opcode = mFlash[_position];
		// TODO: Translate opcode names
		std::cout << std::dec << _position << "\t(0x" << std::hex << (int)_position << ")\t0x" << opcode << "\n";
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::showMemoryCell(unsigned _position) const
	{
		std::cout << "0x" << std::hex << (int)_position << "\t" << unsigned(mDataSpace[_position]) 
		<< std::dec << std::endl;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::executeOneInstruction()
	{
		mCurOpcode = mFlash[mProgramCounter];
		++mTotalSimulatedInstructions;
		return (this->*(mExecutionTable[mProgramCounter++]))();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::updateTimers(unsigned _cycles)
	{
		(void)_cycles;
		// TODO: support timers
	}

	//-------------------------------------------------------------------------------------------------------------------
	uint8_t& Avr8bit::statusRegister()
	{
		return mDataSpace[0x5f];
	}

	//-------------------------------------------------------------------------------------------------------------------
	uint8_t Avr8bit::statusRegister() const
	{
		return mDataSpace[0x5f];
	}

	//-------------------------------------------------------------------------------------------------------------------
	uint16_t Avr8bit::stackPointer() const
	{
		return (unsigned(mDataSpace[0x5e]) << 8) | mDataSpace[0x5d];
	}

	//-------------------------------------------------------------------------------------------------------------------
	void Avr8bit::setStackPointer(uint16_t _sp)
	{
		rev::revAssert(_sp > 0x200, "Error: Trying to set stack pointer below Data SRAM");
		mDataSpace[0x5e] = uint8_t(_sp >> 8);
		mDataSpace[0x5d] = uint8_t(_sp & 0xff);
	}

	//-------------------------------------------------------------------------------------------------------------------
	void Avr8bit::pop(unsigned _n, void * _dst)
	{
		uint8_t * bytes = (uint8_t*)_dst;
		uint16_t SP = stackPointer();
		for(unsigned i = _n; i > 0; --i)
		{
			bytes[i-1] = readSRAM(++SP);
		}
		setStackPointer(SP);
	}

		//-------------------------------------------------------------------------------------------------------------------
	void Avr8bit::push(unsigned _n, void * _data)
	{
		uint8_t * bytes = (uint8_t*)_data;
		uint16_t SP = stackPointer();
		for(unsigned i = 0; i < _n; ++i)
		{
			writeToSRAM(SP--, bytes[i]);
		}
		setStackPointer(SP);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::generateExecutionTable()
	{
		// Create an opcode dispatcher table
		createOpcodeDispatcher();
		// Use the opcode dispatcher to fill the execution table
		for(unsigned i = 0; i < mFlashSize; ++i)
		{
			// Fill the execution table by indexing the dispatcher with the corresponding opcode
			mExecutionTable[i] = mDispatcherTable[mFlash[i] >> 10];
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::createOpcodeDispatcher()
	{
		if(nullptr != mDispatcherTable)
			return; // The table already exists

		mDispatcherTable = new OpcodeDispatcher[64];

		// Fill execution table with invalid instructions
		for(unsigned i = 0; i < 64; ++i)
			mDispatcherTable[i] = &Avr8bit::unsupportedOpcode;

		mDispatcherTable[0x01] = &Avr8bit::CPC;

		mDispatcherTable[0x09] = &Avr8bit::EOR;

		mDispatcherTable[0x0c] = &Avr8bit::CPI;
		mDispatcherTable[0x0d] = &Avr8bit::CPI;
		mDispatcherTable[0x0e] = &Avr8bit::CPI;
		mDispatcherTable[0x0f] = &Avr8bit::CPI;

		mDispatcherTable[0x24] = &Avr8bit::opcode100100;
		mDispatcherTable[0x25] = &Avr8bit::opcode100101;

		mDispatcherTable[0x2c] = &Avr8bit::IN;
		mDispatcherTable[0x2d] = &Avr8bit::IN;
		mDispatcherTable[0x2e] = &Avr8bit::OUT;
		mDispatcherTable[0x2f] = &Avr8bit::OUT;
		mDispatcherTable[0x30] = &Avr8bit::RJMP;
		mDispatcherTable[0x31] = &Avr8bit::RJMP;
		mDispatcherTable[0x32] = &Avr8bit::RJMP;
		mDispatcherTable[0x33] = &Avr8bit::RJMP;

		mDispatcherTable[0x38] = &Avr8bit::LDI;
		mDispatcherTable[0x39] = &Avr8bit::LDI;
		mDispatcherTable[0x3a] = &Avr8bit::LDI;
		mDispatcherTable[0x3b] = &Avr8bit::LDI;

		mDispatcherTable[0x3d] = &Avr8bit::BRBC;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::unsupportedOpcode()
	{
		std::cout << "Unsuported opcode: 0x"	<< std::hex << mCurOpcode << std::dec << std::endl;
		std::cout << "Program counter = " << mProgramCounter-1 << std::endl;
		std::cout << "Total simulated instructions = " << mTotalSimulatedInstructions << std::endl;
		return 0;
	}

	unsigned Avr8bit::opcode100100()
	{
		if((mCurOpcode&0xfe0f) == 0x920f)
			return PUSH();
		else if((mCurOpcode&0xfe0f) == 0x900f)
			return POP();
		else
			return unsupportedOpcode();
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::opcode100101()
	{
		if((mCurOpcode&0xfe0e) == 0x940c)
			return JMP();
		else if((mCurOpcode&0xfe0e)==0x940e)
			return CALL();
		else if(mCurOpcode == 0x9508)
			return RET();
		else if((mCurOpcode&0xff88)==0x9488)
			return BCLR();
		else
			return unsupportedOpcode();
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::BCLR()
	{
		uint8_t s = (mCurOpcode>>4)&0x7;
		statusRegister() &= ~(1<<s);
		return 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::BRBC()
	{
		uint8_t s = mCurOpcode & 0x7;
		uint8_t k = (mCurOpcode>>3) & 0x7f;
		if((statusRegister() & (1<<s)) == 0) // If bit is cleared
		{
			if (k > 63)
				mProgramCounter -= (128 - k);
			else
				mProgramCounter += k;
			return 2;
		}
		else
		{
			return 1;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::CALL()
	{
		unsigned returnAddress = mProgramCounter+1;
		push(3, &returnAddress);
		// Gather function address from the next byte in program memory
		unsigned addressL = mFlash[mProgramCounter];
		unsigned addressH = ((mCurOpcode>>4) & 0x1F) | (mCurOpcode & 0x1);
		// Set program counter to the
		mProgramCounter = (addressH << 8) | addressL;
		mCallStack.push_back(mProgramCounter); // Push called function onto the stack
		rev::revAssert(mProgramCounter < mFlashSize);
		return 5;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::CPC()
	{
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		uint8_t Rd = mDataSpace[d];
		uint8_t r = ((mCurOpcode>>5)&0x10) | (mCurOpcode&0x0f);
		uint8_t Rr = mDataSpace[r];
		unsigned C = statusRegister() & 0x1;
		uint8_t R = Rd - Rr - uint8_t(C);
		unsigned Z = (statusRegister()>>1) & 0x01;
		statusRegister() &= 0xC0;
		unsigned Rr3 = (Rr>>3) & 1;
		unsigned R3 = (R>>3) & 1;
		unsigned Rd3 = (Rd>>3) & 1;
		unsigned H = ((!Rd3)&&Rr3) || (Rr3&&R3) || (R3&&!Rd3);
		unsigned Rr7 = (Rr>>7)&1;
		unsigned R7 = (R>>7)&1;
		unsigned Rd7 = (Rd>>7)&1;
		unsigned V = (Rd7&&(!Rr7)&&!R7)||((!Rd7)&&Rr7&&R7);
		unsigned S = H ^ V;
		unsigned N = R7;
		Z = R==0?Z:0;
		C = ((!Rd7)&&Rr7) || (Rr7&&R7) || (R7&&!Rd7);
		statusRegister() |= (H<<5) | (S<<4) | (V<<3) | (N<<2) | (Z<<1) | C;
		return 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::CPI()
	{
		uint8_t d = 0x10 |((mCurOpcode>>4) & 0xf);
		uint8_t Rd = mDataSpace[d];
		uint8_t K = ((mCurOpcode>>4) & 0xf0) | (mCurOpcode & 0xf);
		uint8_t R = Rd - K;
		statusRegister() &= 0xC0;
		unsigned K3 = (K>>3) & 1;
		unsigned R3 = (R>>3) & 1;
		unsigned Rd3 = (Rd>>3) & 1;
		unsigned H = ((!Rd3)&&K3) || (K3&&R3) || (R3&&!Rd3);
		unsigned K7 = (K>>7)&1;
		unsigned R7 = (R>>7)&1;
		unsigned Rd7 = (Rd>>7)&1;
		unsigned V = (Rd7&&(!K7)&&!R7)||((!Rd7)&&K7&&R7);
		unsigned S = H ^ V;
		unsigned N = R7;
		unsigned Z = R==0?1:0;
		unsigned C = ((!Rd7)&&K7) || (K7&&R7) || (R7&&!Rd7);
		statusRegister() |= (H<<5) | (S<<4) | (V<<3) | (N<<2) | (Z<<1) | C;
		return 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::EOR()
	{
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		uint8_t r = ((mCurOpcode>>5) & 0x10) | (mCurOpcode & 0x0f);
		mDataSpace[d] = mDataSpace[d] ^ mDataSpace[r];
		// Update status register
		statusRegister() &= 0xE1;
		uint8_t N = ((mDataSpace[d]>>7) & 1)? 1 : 0;
		uint8_t S = N;
		uint8_t Z = (mDataSpace[d] == 0x00) ? 1 : 0;
		statusRegister() |= (S<<4) | (N<<2) | (Z<<1);
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::IN()
	{
		uint8_t d = (mCurOpcode & 0x1f0) >> 4;
		uint8_t A = ((mCurOpcode & 0x600) >> 5) | (mCurOpcode & 0x0f);
		mDataSpace[d] = (uint8_t) mIOMemory.read(A);
		return 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::JMP()
	{
		unsigned addressH = ((mCurOpcode & 0x01f1) >> 3) | (mCurOpcode & 0x1);
		unsigned addressL = mFlash[mProgramCounter];
		mProgramCounter = (addressH << 16) | addressL;
		rev::revAssert(mProgramCounter < mFlashSize);
		return 3;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::LDI()
	{
		uint8_t data = ((mCurOpcode & 0x0f00)>>4) | (mCurOpcode & 0x000f);
		uint8_t r = 0x10 | ((mCurOpcode & 0x00f0) >> 4);
		mDataSpace[r] = data;
		return 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::OUT()
	{
		uint8_t r = (mCurOpcode & 0x1f0) >> 4;
		uint8_t A = ((mCurOpcode & 0x600) >> 5) | (mCurOpcode & 0x0f);
		mIOMemory.write(A, mDataSpace[r]);
		return 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::POP()
	{
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		pop(1, &mDataSpace[d]);
		return 2;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::PUSH()
	{
		uint8_t r = (mCurOpcode>>4) & 0x1f;
		push(1, &mDataSpace[r]);
		return 2;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::RET()
	{
		mCallStack.pop_back();
		pop(3, &mProgramCounter);
		rev::revAssert(mProgramCounter < mFlashSize);
		return 5;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::RJMP()
	{
		uint16_t hexOffset = uint16_t(mCurOpcode & 0xfff);
		int16_t relAddress;
		if(hexOffset > 2047)
			relAddress = hexOffset - 4096; // = -(4096 - hexOffset), Two's complement
		else
			relAddress = hexOffset;
		mProgramCounter += relAddress;
		rev::revAssert(mProgramCounter < mFlashSize);
		return 2;
	}

}	// namespace component
}	// namespace rose