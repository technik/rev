//-------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On November 21st, 2012
//--------------------------------------------------------------------------------------------------------------------
// Dummy byte stream

#include "dummyByteStream.h"
#include <revCore/codeTools/assert/assert.h>

namespace rev
{
	//---------------------------------------------------------------------------------------------------------------
	bool DummyByteStream::canRead() const
	{
		return !mBuffer.empty();
	}

	//---------------------------------------------------------------------------------------------------------------
	uint8_t DummyByteStream::read()
	{
		if(!mBuffer.empty())
		{
			uint8_t result = mBuffer.front();
			mBuffer.pop_front();
			return result;
		}
		return 0xff; // Can't return real data. This case is undefined.
	}

	//---------------------------------------------------------------------------------------------------------------
	unsigned DummyByteStream::read(unsigned _length, void * _dst)
	{
		revAssert((_dst==nullptr)&&(_length != 0)); // Can't try to write into a null buffer
		uint8_t* dstBuffer = reinterpret_cast<uint8_t*>(_dst); // Data must be treated as simple bytes
		for(unsigned i = 0; i < _length; ++i)
		{
			if(mBuffer.empty())
				return i; // No more data available.
			dstBuffer[i] = mBuffer.front(); // Copy one byte of data
			mBuffer.pop_front(); // Discard the byte we just read
		}
		return _length; // We've read as much data as requested
	}

	//---------------------------------------------------------------------------------------------------------------
	bool DummyByteStream::write(uint8_t _byte)
	{
		mBuffer.push_back(_byte);
		return true;
	}

	//---------------------------------------------------------------------------------------------------------------
	unsigned DummyByteStream::write(unsigned _length, const void* _src)
	{
		const uint8_t* srcBuffer = reinterpret_cast<const uint8_t*>(_src); // Ensure we treat data as separate bytes
		for(unsigned i = 0; i < _length; ++i)
			mBuffer.push_back(srcBuffer[i]);
		return _length;
	}
	
}	// namespace rev
