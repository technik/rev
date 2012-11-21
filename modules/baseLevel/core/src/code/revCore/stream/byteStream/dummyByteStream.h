//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On November 21st, 2012
//----------------------------------------------------------------------------------------------------------------------
// Dummy byte stream
// This byte stream exists only for unit-testing purposes.

#ifndef _REV_CORE_STREAM_BYTESTREAM_DUMMYBYTESTREAM_H_
#define _REV_CORE_STREAM_BYTESTREAM_DUMMYBYTESTREAM_H_

#include <list>

#include "byteStream.h"

namespace rev
{
	// Implements a simple FIFO queue to simulate a byte stream.
	class DummyByteStream: public IOByteStream
	{
	public:
		bool		canRead () const;
		uint8_t		read	();
		unsigned	read	(unsigned _length, void* _dst);
		bool		write	(uint8_t _byte);
		unsigned	write	(unsigned _length, const void* _src);
	private:
		std::list<uint8_t>	mBuffer;
	};
}	// namespace rev

#endif // _REV_CORE_STREAM_BYTESTREAM_DUMMYBYTESTREAM_H_
