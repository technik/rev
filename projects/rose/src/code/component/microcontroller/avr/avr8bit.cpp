//----------------------------------------------------------------------------------------------------------------------
// Rose project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On October 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// 8-bit AVR Microcontroller

#include "avr8bit.h"

#include <cstdint>

#include <codeTools/assert/assert.h>
#include <file/file.h>
using rev::File;

namespace rose { namespace component {

	//------------------------------------------------------------------------------------------------------------------
	Avr8bit::Avr8bit(unsigned _flash, unsigned _ram)
		:mFlashSize(_flash * 512) // slots = _kilobytes * 1024 / 2.
		,mDataSize(_ram * 1024)
	{
		mFlash = new uint16_t[mFlashSize];
		mDataSpace = new uint8_t[mDataSize + 0x200]; // Data memory + register file + i/o memory + extended i/o memory.
	}

	//------------------------------------------------------------------------------------------------------------------
	Avr8bit::~Avr8bit()
	{
		delete[] mFlash;
		delete[] mDataSpace;
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
		const char * hexCode = hexFile->bufferAsText();
		bool reachedEndOfFile = false;
		unsigned hexLineIndex = 0;
		while(! reachedEndOfFile)
		{
			reachedEndOfFile = processHexLine(hexCode, hexLineIndex);
		}
		delete hexFile;
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Avr8bit::reset()
	{
		for(unsigned i = 0; i < mDataSize+0x200; ++i)
			mDataSpace[0] = 0x00;
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

}	// namespace component
}	// namespace rose