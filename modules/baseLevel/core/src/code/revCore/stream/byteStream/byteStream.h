//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On November 21st, 2012
//----------------------------------------------------------------------------------------------------------------------
// Byte streams

#ifndef _REV_CORE_STREAM_BYTESTREAM_BYTESTREAM_H_
#define _REV_CORE_STREAM_BYTESTREAM_BYTESTREAM_H_

#include <cstdint>

namespace rev {

	// Input byte stream interface
	class InputByteStream
	{
	public:
		virtual	bool		canRead () const = 0;								///\ True if there is any data to be read
		virtual uint8_t		read	() = 0;										///\ Reads one byte from the stream
																				///\ Return is undefined when no data can be retrieved, so calling
																				///\ canRead() before read() is usually a good idea.
		virtual unsigned	read	(unsigned _length,  void* _dst) = 0;		///\ Reads _length bytes from the string and writes them into _dst
																				///\ Notice _dst must have enough prelocated space to store _length bytes
																				///\ Returns the number of bytes the stream was able to retrieve
	};

	// Output byte stream interface
	class OutputByteStream
	{
	public:
		virtual bool		write	(uint8_t _byte) = 0;						///\ Writes a single byte to the stream. Returns true on success.
		virtual unsigned	write	(unsigned _length, const void* _src) = 0;	///\ Writes _length bytes from _src into the stream
																				///\ Returns the number of bytes that were effectively written to the stream.
	};

	// Input/Output byte stream
	class IOByteStream : public InputByteStream, public OutputByteStream
	{}; // Just a wrapper for streams that support bot input and output.
}	// namespace rev

#endif // _REV_CORE_STREAM_BYTESTREAM_BYTESTREAM_H_