//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On October 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// 8-bit AVR Microcontroller

#include "avr8bit.h"

#include <cstdint>
#include <iostream>
#include <sstream>

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/file/file.h>
using rev::File;
using rev::revLog;

using namespace std;

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

		// Debug information
		mOpcodeNames = new string[mFlashSize];
	}

	//------------------------------------------------------------------------------------------------------------------
	Avr8bit::~Avr8bit()
	{
		delete[] mFlash;
		delete[] mDataSpace;
		delete[] mExecutionTable;
		delete[] mOpcodeNames;
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
		generateOpcodeNames();
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::reset()
	{
		for(unsigned i = 0; i < mDataSize; ++i)
			mDataSpace[i] = 0x00;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::setBreakpoint(unsigned _instruction)
	{
		mBreakpoints.insert(_instruction);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::removeBreakpoint(unsigned _instruction)
	{
		mBreakpoints.erase(_instruction);
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
		std::cout << "Program Counter = " << dec << mProgramCounter << std::endl
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
	unsigned Avr8bit::step()
	{
		// TODO: Support interrupts
		unsigned elapsedCycles = executeOneInstruction();
		showAssemblyInstruction(mProgramCounter);
		return elapsedCycles;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::run()
	{
		unsigned cycles = 0;
		while(mBreakpoints.find(mProgramCounter) == mBreakpoints.end())
			cycles += step();
		showExecutionStatus();
		return cycles;
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
		cout << dec << _position << "\t(0x" << hex << (int)_position << dec << ")\t0x" << opcode;
		cout << "\t" << mOpcodeNames[_position] << endl;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::showMemoryCell(unsigned _position) const
	{
		cout << "0x" << hex << (int)_position << "\t" << unsigned(mDataSpace[_position]) 
		<< dec << endl;
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

	//-------------------------------------------------------------------------------------------------------------------
	bool Avr8bit::isTwoWordInstruction(unsigned _opcode)
	{
		unsigned maskedOpcode = _opcode & 0xfe0f;
		return (((maskedOpcode&0xfc0f)==0x9000) // STS, LDS
			 || ((maskedOpcode&0xfe0c)==0x940c)); // CALL, JMP
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
	void Avr8bit::generateOpcodeNames()
	{
		// Initialize tha table
		for(unsigned i = 0; i < mFlashSize; ++i)
		{
			uint16_t opcode = mFlash[i];
			mOpcodeNames[i] = "--- Unknown Opcode ---";
			unsigned hash = opcode >> 10;
			switch(hash)
			{
				case 0:
				{
					if(0x00 == opcode)
						mOpcodeNames[i] = "NOP: No operation";
					else if((0x0300&opcode)==0x0100)
					{
						uint8_t r = (opcode & 0xf) << 1;
						uint8_t d = (opcode & 0xf0) >> 3;
						stringstream name;
						name << "MOVW d:" << (int)d << " <- r:" << (int)r;
						mOpcodeNames[i] = name.str();
					}
					break;
				}
				case 1:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "CPC d:" << (int)d << " - r:" << (int)r << " - C";
					mOpcodeNames[i] = name.str();
					break;
				}
				case 2:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "SBC d <- d:" << (int)d << " - r:" << (int)r << " - C";
					mOpcodeNames[i] = name.str();
					break;
				}
				case 3:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "ADD d:" << (int)d << " += r:" << (int)r;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 4:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "CPSE if d:" << (int)d << "==r:" << (int)r << " -> Skip";
					mOpcodeNames[i] = name.str();
					break;
				}
				case 5:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "CP d:" << (int)d << " - r:" << (int)r;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 6:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "SUB d:" << (int)d << " -= r:" << (int)r;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 7:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "ADC d:" << (int)d << " r:" << (int)r << " + C";
					mOpcodeNames[i] = name.str();
					break;
				}
				case 8:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "AND d <- d:" << (int)d << " & r:" << (int)r;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 9:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "EOR d <- d:" << (int)d << " ^ r:" << (int)r;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 10:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "OR d <- d:" << (int)d << " | r:" << (int)r;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 11:
				{
					stringstream name;
					uint8_t d = (opcode>>4) & 0x1f;
					uint8_t r = ((opcode>>5)&0x10) | (opcode&0x0f);
					name << "MOV d:" << (int)d << " <- r:" << (int)r;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 12:
				case 13:
				case 14:
				case 15:
				{
					stringstream name;
					uint8_t d = 0x10 |((opcode>>4) & 0xf);
					uint8_t K = ((opcode>>4) & 0xf0) | (opcode & 0xf);
					name << "CPI d:" << (int)d << " - K:" << (int)K;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 16:
				case 17:
				case 18:
				case 19:
				{
					stringstream name;
					uint8_t d = 0x10 |((opcode>>4) & 0xf);
					uint8_t K = ((opcode>>4) & 0xf0) | (opcode & 0xf);
					name << "SBCI d <- d:" << (int)d << " - K:" << (int)K << " -C";
					mOpcodeNames[i] = name.str();
					break;
				}
				case 20:
				case 21:
				case 22:
				case 23:
				{
					stringstream name;
					uint8_t d = 0x10 |((opcode>>4) & 0xf);
					uint8_t K = ((opcode>>4) & 0xf0) | (opcode & 0xf);
					name << "SUBI d <- d:" << (int)d << " - K:" << (int)K;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 24:
				case 25:
				case 26:
				case 27:
				{
					stringstream name;
					uint8_t d = 0x10 |((opcode>>4) & 0xf);
					uint8_t K = ((opcode>>4) & 0xf0) | (opcode & 0xf);
					name << "ORI d <- d:" << (int)d << " | K:" << (int)K;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 28:
				case 29:
				case 30:
				case 31:
				{
					stringstream name;
					uint8_t d = 0x10 |((opcode>>4) & 0xf);
					uint8_t K = ((opcode>>4) & 0xf0) | (opcode & 0xf);
					name << "ANDI d <- d:" << (int)d << " & K:" << (int)K;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 32:
				{
					stringstream name;
					if(opcode&0x0200)
					{
						int r = (opcode>>4)&0x1f;
						name << "STZ(1) (Z++) <- r:" << r;
						mOpcodeNames[i] = name.str();
					}
					break;
				}
				case 36:
				{
					stringstream name;
					int r = (opcode>>4)&0x1f;
					if((opcode&0xfe0f)==0x9007) // ELPM3
					{
						name << "ELPM(3) d:" << r << " <- *((RAMPZ:Z)++)";
						mOpcodeNames[i] = name.str();
					}
					else if((opcode&0xfe0f)==0x920d)
					{
						name << "ST (X++) <- r:" << r;
						mOpcodeNames[i] = name.str();
					}else if((opcode&0xfe0f) == 0x920f)
					{
						name << "PUSH (SP--) <- r:" << r;
						mOpcodeNames[i] = name.str();
					}
					else if((opcode&0xfe0f) == 0x900f)
					{
						name << "POP d:" << r << " <- (++SP))";
						mOpcodeNames[i] = name.str();
					}
					break;
				}
				case 37:
				{
					stringstream name;
					//int s = (opcode>>4)&0x7;
					//int K = (opcode>>4)&0xf;
					//int d = (opcode>>4)&0x1f;
					unsigned k = ((opcode&0x1f0)<<13)|((opcode&0x01)<<16)|mFlash[i+1];
					if((opcode&0xfe0e)==0x940c)
					{
						name << "JMP PC <- k:" << k;
						if(k == 0)
							name << " // Reset";
						mOpcodeNames[i] = name.str();
						++i;
					}
					else if((opcode&0xfe0e)==0x940e)
					{
						name << "CALL PC <- k:" << k;
						mOpcodeNames[i] = name.str();
						++i;
					}
					else if(opcode == 0x9508)
					{
						mOpcodeNames[i] = "RET";
					}
					else if((opcode&0xff8f)==0x9488)
					{
						name << "BLCR s:" << ((opcode>>4)&0x7);
						mOpcodeNames[i] = name.str();
					}
					break;
				}
				case 44:
				case 45:
				{
					stringstream name;
					int d = (opcode>>4) & 0x1f;
					int A = ((opcode >> 5)&0x30) | (opcode&0xf);
					name << "IN d:" << d << " <- A:0x" << hex << A << " (0x" << A+0x20  << dec << ")";
					mOpcodeNames[i] = name.str();
					break;
				}
				case 46:
				case 47:
				{
					stringstream name;
					int d = (opcode>>4) & 0x1f;
					int A = ((opcode >> 5)&0x30) | (opcode&0xf);
					name << "OUT A:0x" << hex << A << " (0x" << A+0x20  << dec <<") <- d:" << d;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 48:
				case 49:
				case 50:
				case 51:
				{
					stringstream name;
					int16_t k = opcode&0xfff;
					if(k&0x800) // negative
						k |= 0xf000; // Extend sign
					name << "RJMP PC += " << int(k);
					mOpcodeNames[i] = name.str();
					break;
				}
				case 56:
				case 57:
				case 58:
				case 59:
				{
					stringstream name;
					int d = 0x10 | ((opcode>>4)&0x0f);
					int K = ((opcode>>4)&0xf0)|(opcode&0xf);
					name << "LDI d:" << d << " <- " << K << hex << "(0x" << K << ")" << dec;
					mOpcodeNames[i] = name.str();
					break;
				}
				case 61:
				{
					stringstream name;
					uint8_t s = mCurOpcode & 0x7;
					uint8_t k = (mCurOpcode>>3) & 0x7f;
					name << "BRBC s:" << (int)s << ", k:"<< (int)k;
					mOpcodeNames[i] = name.str();
					break;
				}
			}
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

		mDispatcherTable[0x00] = &Avr8bit::opcode000000;
		mDispatcherTable[0x01] = &Avr8bit::CPC;
		mDispatcherTable[0x03] = &Avr8bit::ADD;
		mDispatcherTable[0x04] = &Avr8bit::CPSE;
		mDispatcherTable[0x05] = &Avr8bit::CP;
		mDispatcherTable[0x06] = &Avr8bit::SUB;
		mDispatcherTable[0x07] = &Avr8bit::ADC;
		mDispatcherTable[0x08] = &Avr8bit::AND;
		mDispatcherTable[0x09] = &Avr8bit::EOR;
		mDispatcherTable[0x0a] = &Avr8bit::OR;
		mDispatcherTable[0x0b] = &Avr8bit::MOV;
		mDispatcherTable[0x0c] = &Avr8bit::CPI;
		mDispatcherTable[0x0d] = &Avr8bit::CPI;
		mDispatcherTable[0x0e] = &Avr8bit::CPI;
		mDispatcherTable[0x0f] = &Avr8bit::CPI;
		mDispatcherTable[0x10] = &Avr8bit::SBCI;
		mDispatcherTable[0x11] = &Avr8bit::SBCI;
		mDispatcherTable[0x12] = &Avr8bit::SBCI;
		mDispatcherTable[0x13] = &Avr8bit::SBCI;
		mDispatcherTable[0x14] = &Avr8bit::SUBI;
		mDispatcherTable[0x15] = &Avr8bit::SUBI;
		mDispatcherTable[0x16] = &Avr8bit::SUBI;
		mDispatcherTable[0x17] = &Avr8bit::SUBI;
		mDispatcherTable[0x18] = &Avr8bit::ORI;
		mDispatcherTable[0x19] = &Avr8bit::ORI;
		mDispatcherTable[0x1a] = &Avr8bit::ORI;
		mDispatcherTable[0x1b] = &Avr8bit::ORI;
		//mDispatcherTable[0x1c] = &Avr8bit::ANDI;
		//mDispatcherTable[0x1d] = &Avr8bit::ANDI;
		//mDispatcherTable[0x1e] = &Avr8bit::ANDI;
		//mDispatcherTable[0x1f] = &Avr8bit::ANDI;
		mDispatcherTable[0x20] = &Avr8bit::opcode100000;

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
		mDispatcherTable[0x3c] = &Avr8bit::BRBS;
		mDispatcherTable[0x3d] = &Avr8bit::BRBC;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::unsupportedOpcode()
	{
		std::cout << "Unsuported opcode: 0x"	<< std::hex << mCurOpcode << std::dec << std::endl;
		showAssemblyInstruction(mProgramCounter-1);
		std::cout << "Program counter = " << mProgramCounter-1 << std::endl;
		std::cout << "Total simulated instructions = " << mTotalSimulatedInstructions << std::endl;
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::opcode000000()
	{
		if(0x00 == mCurOpcode)
			return 1; // NOP
		else if((mCurOpcode&0xfe00)==0x00)
			return MOVW();
		else
			return unsupportedOpcode();
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::opcode100000()
	{
		if((mCurOpcode&0xfe0f)==0x8200)
			return STZ1();
		else if((mCurOpcode&0xfe0f)==0x8000)
			return LDZ1();
		else
			return unsupportedOpcode();
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::opcode100100()
	{
		if((mCurOpcode&0xfe0f) == 0x920f)
			return PUSH();
		else if((mCurOpcode&0xfe0f) == 0x900f)
			return POP();
		else if((mCurOpcode&0xfe0f) == 0x920d)
			return ST();
		else if((mCurOpcode&0xfe0c)==0x9004)
			return LPM();
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
		else if((mCurOpcode&0xff8f)==0x9408)
			return BSET();
		else if((mCurOpcode&0xff8f)==0x9400)
			return COM();
		else
			return unsupportedOpcode();
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::ADC()
	{
		// {CProfileFunction profile("ADC"); }
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		uint8_t Rd = mDataSpace[d];
		uint8_t r = ((mCurOpcode>>5) & 0x10) | (mCurOpcode & 0x0f);
		uint8_t Rr = mDataSpace[r];
		unsigned C = statusRegister() & 1;
		uint8_t R = Rd + Rr + uint8_t(C);
		statusRegister() &= 0xC0;
		unsigned Rr3 = (Rr>>3) & 1;
		unsigned R3 = (R>>3) & 1;
		unsigned Rd3 = (Rd>>3) & 1;
		unsigned H = (Rd3&&Rr3) || (Rr3&&!R3) || (Rd3&&!R3);
		unsigned Rr7 = (Rr>>7)&1;
		unsigned R7 = (R>>7)&1;
		unsigned Rd7 = (Rd>>7)&1;
		unsigned V = (Rd7&&Rr7&&!R7)||((!Rd7)&&(!Rr7)&&R7);
		unsigned S = H ^ V;
		unsigned N = R7;
		unsigned Z = R==0?1:0;
		C = (Rd7&&Rr7) || (Rr7&&!R7) || (Rd7&&!R7);
		statusRegister() |= (H<<5) | (S<<4) | (V<<3) | (N<<2) | (Z<<1) | C;
		mDataSpace[d] = R;
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::ADD()
	{
		// { CProfileFunction profile("ADD"); }
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		uint8_t r = ((mCurOpcode>>5) & 0x10) | (mCurOpcode & 0x0f);
		uint8_t Rd = mDataSpace[d];
		uint8_t Rr = mDataSpace[r];
		uint8_t R = Rd + Rr;
		statusRegister() &= 0xC0;
		unsigned Rr3 = (Rr>>3) & 1;
		unsigned R3 = (R>>3) & 1;
		unsigned Rd3 = (Rd>>3) & 1;
		unsigned H = (Rd3&&Rr3) || (Rr3&&!R3) || (Rd3&&!R3);
		unsigned Rr7 = (Rr>>7)&1;
		unsigned R7 = (R>>7)&1;
		unsigned Rd7 = (Rd>>7)&1;
		unsigned V = (Rd7&&Rr7&&!R7)||((!Rd7)&&(!Rr7)&&R7);
		unsigned S = H ^ V;
		unsigned N = R7;
		unsigned Z = R==0?1:0;
		unsigned C = (Rd7&&Rr7) || (Rr7&&!R7) || (Rd7&&!R7);
		statusRegister() |= (H<<5) | (S<<4) | (V<<3) | (N<<2) | (Z<<1) | C;
		mDataSpace[d] = R;
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::AND()
	{
		// { CProfileFunction profile("AND"); }
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		uint8_t r = ((mCurOpcode>>5) & 0x10) | (mCurOpcode & 0x0f);
		uint8_t Rd = mDataSpace[d];
		uint8_t Rr = mDataSpace[r];
		mDataSpace[d] = Rd & Rr;
		// Update status register
		statusRegister() &= 0xE1;
		uint8_t N = ((mDataSpace[d]>>7) & 1)? 1 : 0;
		uint8_t S = N;
		uint8_t Z = (mDataSpace[d]) == 0x00 ? 1 : 0;
		statusRegister() |= (S<<4) | (N<<2) | (Z<<1);
		return 1;
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

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::BRBS()
	{
		//CProfileFunction profile("BRBS");
		uint8_t s = mCurOpcode & 0x7;
		uint8_t k = (mCurOpcode>>3) & 0x7f;
		if((statusRegister() & (1<<s)) != 0) // If bit is set
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

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::BSET()
	{
		//CProfileFunction profile("BSET");
		uint8_t s = (mCurOpcode>>4)&0x7;
		statusRegister() |= (1<<s);
		return 1;
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

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::COM()
	{
		// { CProfileFunction profile("COM"); }
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		uint8_t R = 0xff - mDataSpace[d];
		uint8_t N = (R>>7) & 1;
		uint8_t S = N;
		uint8_t Z = (R==0x00)?1:0;
		statusRegister() &= 0xE1;
		statusRegister() |= (S<<4) | (N<<2) | (Z<<1) | 1;
		mDataSpace[d] = R;
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::CP()
	{
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		uint8_t Rd = mDataSpace[d];
		uint8_t r = ((mCurOpcode>>5)&0x10) | (mCurOpcode&0x0f);
		uint8_t Rr = mDataSpace[r];
		uint8_t R = Rd - Rr;
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
		unsigned Z = R==0?1:0;
		unsigned C = ((!Rd7)&&Rr7) || (Rr7&&R7) || (R7&&!Rd7);
		statusRegister() |= (H<<5) | (S<<4) | (V<<3) | (N<<2) | (Z<<1) | C;
		return 1;
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

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::CPSE()
	{
		// CProfileFunction profile("CPC");
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		uint8_t Rd = mDataSpace[d];
		uint8_t r = ((mCurOpcode>>5)&0x10) | (mCurOpcode&0x0f);
		uint8_t Rr = mDataSpace[r];
		if(Rr == Rd)
		{
			if(isTwoWordInstruction(mFlash[mProgramCounter]))
			{
				mProgramCounter += 2;
				return 3;
			}
			else
			{
				++mProgramCounter;
				return 2;
			}
		}
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::DEC()
	{
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		uint8_t rd = mDataSpace[d];
		uint8_t R = rd-1;
		uint8_t V = (R==0x7f)?1:0;
		uint8_t N = R>>7;
		uint8_t Z = R?0:2;
		uint8_t S = (V^N)&0x01;
		mDataSpace[d] = R;
		statusRegister() = (statusRegister()&0xe1) | (S<<4) | (V<<3) | (N<<2) | Z;
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::EICALL()
	{
		// Push return address
		unsigned returnAddress = mProgramCounter+1;
		push(3, &returnAddress);
		// Gather destination from extended z pointer
		unsigned addressL = mDataSpace[0x1e]; // Z Low
		unsigned addressH = mDataSpace[0x1f]; // Z High
		unsigned addressHH= mIOMemory.read(0x3c); // EIND
		// Set program counter to the new address
		mProgramCounter = (addressHH << 16) | (addressH << 8) | addressL;
		mCallStack.push_back(mProgramCounter);
		rev::revAssert(mProgramCounter < mFlashSize);
		return 4;
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
		uint8_t d = uint8_t((mCurOpcode & 0x1f0) >> 4);
		uint8_t A =  uint8_t(((mCurOpcode & 0x600) >> 5) | (mCurOpcode & 0x0f));
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
		uint8_t data =  uint8_t(((mCurOpcode & 0x0f00)>>4) | (mCurOpcode & 0x000f));
		uint8_t r =  uint8_t(0x10 | ((mCurOpcode & 0x00f0) >> 4));
		mDataSpace[r] = data;
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::LDS()
	{
		uint8_t d = (mCurOpcode>>4)&0x1f;
		unsigned k = mFlash[mProgramCounter++];
		mDataSpace[d] = readSRAM(k);
		return 2;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::LDZ1()
	{
		// { CProfileFunction profile("LDZ1"); }
		unsigned Z = (mIOMemory.read(0x3b)<<16)|(mDataSpace[0x1f])|(mDataSpace[0x1e]);
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		mDataSpace[d] = readSRAM(Z);
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::LPM() // Must support lpm2, lpm3, elpm2 and elmp3
	{
		uint8_t d = (mCurOpcode>>4)&0x1f;
		bool extended = mCurOpcode & 0x02 ? true : false;
		unsigned Z = (extended?(mIOMemory.read(0x3b)<<16):0) | (mDataSpace[0x1f]<<8) | mDataSpace[0x1e];
		char * flash = reinterpret_cast<char*>(mFlash);
		mDataSpace[d] = flash[Z];
		if(mCurOpcode & 0x01) // Increment Z
		{
			++Z;
			mDataSpace[0x1e] = Z & 0xff;
			mDataSpace[0x1f] = (Z>>8) & 0xff;
			mIOMemory.write(0x3b, (Z>>16) & 0x03);
		}
		return 3;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::MOV()
	{
		// { CProfileFunction profile("MOV"); }
		uint8_t d = (mCurOpcode>>4) & 0x1f;
		uint8_t r = ((mCurOpcode>>5) & 0x10) | (mCurOpcode & 0x0f);
		mDataSpace[d] = mDataSpace[r];
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::MOVW()
	{
		uint8_t r = (mCurOpcode & 0xf) << 1;
		uint8_t d = (mCurOpcode & 0xf0) >> 3;
		mDataSpace[d++] = mDataSpace[r++];
		mDataSpace[d] = mDataSpace[r];
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::OR()
	{
		// { CProfileFunction profile("OR"); }
		uint8_t d = char((mCurOpcode>>4) & 0x1f);
		uint8_t r = char(((mCurOpcode>>5) & 0x10) | (mCurOpcode & 0x0f));
		uint8_t Rd = mDataSpace[d];
		uint8_t Rr = mDataSpace[r];
		mDataSpace[d] = Rd | Rr;
		// Update status register
		statusRegister() &= 0xE1;
		uint8_t N = ((mDataSpace[d]>>7) & 1)? 1 : 0;
		uint8_t S = N;
		uint8_t Z = (mDataSpace[d]) == 0x00 ? 1 : 0;
		statusRegister() |= (S<<4) | (N<<2) | (Z<<1);
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::ORI()
	{
		//CProfileFunction profile("ORI");
		uint8_t d = char((mCurOpcode>>4) & 0xf) | 0x10;
		uint8_t K = char(((mCurOpcode>>4)& 0xf0) | (mCurOpcode&0x0f));
		uint8_t Rd = mDataSpace[d];
		uint8_t R = Rd | K;
		statusRegister() &= 0xE1;
		unsigned R7 = (R>>7)&1;
		unsigned N = R7;
		unsigned S = N;
		unsigned Z = R==0?1:0;
		statusRegister() |= (S<<4) | (N<<2) | (Z<<1);
		mDataSpace[d] = R;
		return 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::OUT()
	{
		uint8_t r =  uint8_t((mCurOpcode & 0x1f0) >> 4);
		uint8_t A =  uint8_t(((mCurOpcode & 0x600) >> 5) | (mCurOpcode & 0x0f));
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

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::SBC()
	{
		//CProfileFunction profile("SBC");
		uint8_t d = (mCurOpcode>>4)&0x1f;
		uint8_t r = ((mCurOpcode>>5)&0x10) | (mCurOpcode & 0x0f);
		uint8_t Rd = mDataSpace[d];
		uint8_t Rr = mDataSpace[r];
		unsigned C = statusRegister() & 0x1;
		uint8_t R = Rd - Rr - uint8_t(C);
		unsigned Z = (statusRegister() >> 1) & 0x01;
		statusRegister() &= 0xC0;
		unsigned K3 = (Rr>>3) & 1;
		unsigned R3 = (R>>3) & 1;
		unsigned Rd3 = (Rd>>3) & 1;
		unsigned H = ((!Rd3)&&K3) || (K3&&R3) || (R3&&!Rd3);
		unsigned K7 = (Rr>>7)&1;
		unsigned R7 = (R>>7)&1;
		unsigned Rd7 = (Rd>>7)&1;
		unsigned V = (Rd7&&(!K7)&&!R7)||((!Rd7)&&K7&&R7);
		unsigned S = H ^ V;
		unsigned N = R7;
		Z = R==0?Z:0;
		C = ((!Rd7)&&K7) || (K7&&R7) || (R7&&!Rd7);
		statusRegister() |= (H<<5) | (S<<4) | (V<<3) | (N<<2) | (Z<<1) | C;
		mDataSpace[d] = R;
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::SBCI()
	{
		uint8_t d = 0x10 | ((mCurOpcode >> 4) & 0xf);
		uint8_t Rd = mDataSpace[d];
		uint8_t K = ((mCurOpcode >> 4) & 0xf0) | (mCurOpcode & 0xf);
		unsigned C = statusRegister() & 0x1;
		uint8_t R = Rd - K - uint8_t(C);
		unsigned Z = (statusRegister() >> 1) & 0x01;
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
		Z = R==0?Z:0;
		C = ((!Rd7)&&K7) || (K7&&R7) || (R7&&!Rd7);
		statusRegister() |= (H<<5) | (S<<4) | (V<<3) | (N<<2) | (Z<<1) | C;
		mDataSpace[d] = R;
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::ST()
	{
		uint8_t r =  uint8_t((mCurOpcode&0x1f0)>>4);
		uint8_t pointerSel =  uint8_t((mCurOpcode>>2) & 0x3);
		uint8_t rPointer = pointerSel?(0x20-(2*pointerSel)):0x1e;
		unsigned pointer = (mDataSpace[rPointer+1]<<8) | mDataSpace[rPointer];

		pointer -= (mCurOpcode & 0x2) ? 1:0;
		writeToSRAM(pointer & 0xffff, mDataSpace[r]);
		pointer += (mCurOpcode & 0x1) ? 1:0;

		mDataSpace[rPointer] = pointer & 0xff;
		mDataSpace[rPointer+1] = (pointer>>8) & 0xff;

		return 2;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::STS()
	{
		uint8_t r =  uint8_t((mCurOpcode&0x1f0)>>4);
		unsigned k = mFlash[mProgramCounter++];
		writeToSRAM(k, mDataSpace[r]);
		return 2;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::STZ1()
	{
		uint8_t r = (mCurOpcode>>4) & 0x1f;
		unsigned Z = (mIOMemory.read(0x3b)<<16) |(mDataSpace[0x1f]<<8) | mDataSpace[0x1e];
		writeToSRAM(Z, mDataSpace[r]);

		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::SUB()
	{
		uint8_t d = (mCurOpcode >> 4) & 0x1f;
		uint8_t Rd = mDataSpace[d];
		uint8_t r = ((mCurOpcode & 0x0f) | ((mCurOpcode >> 5) & 0x10));
		uint8_t Rr = mDataSpace[r];
		uint8_t R = Rd - Rr;
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
		unsigned Z = R==0?1:0;
		unsigned C = ((!Rd7)&&Rr7) || (Rr7&&R7) || (R7&&!Rd7);
		statusRegister() |= (H<<5) | (S<<4) | (V<<3) | (N<<2) | (Z<<1) | C;
		mDataSpace[d] = R;
		return 1;
	}

	//-------------------------------------------------------------------------------------------------------------------
	unsigned Avr8bit::SUBI()
	{
		uint8_t d = 0x10 | ((mCurOpcode >> 4) & 0xf );
		uint8_t Rd = mDataSpace[d];
		uint8_t K = ((mCurOpcode >> 4) & 0xf0) | (mCurOpcode & 0xf);
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
		mDataSpace[d] = R;
		return 1;
	}

}	// namespace component
}	// namespace rose