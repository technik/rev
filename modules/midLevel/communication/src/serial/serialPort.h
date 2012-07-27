//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Serial port communications

#ifndef _REV_COMM_SERIAL_SERIALPORT_H_
#define _REV_COMM_SERIAL_SERIALPORT_H_

// Platform dependent configuration
#if defined(WIN32) || defined(USE_ARDUINO_LIBS)
#define REV_ENABLE_SERIAL_PORT
#endif // WIN32

// Headers
#include <numericTypes.h>
#include <codeTools/usefulMacros.h>

#ifdef REV_ENABLE_SERIAL_PORT

namespace rev { namespace comm
{
	class SerialPort
	{
	public:
		// Construction and destruction
		SerialPort(u8 port, u32 baudRate, u8 parityBits = 0, u8 stopBits = 0);
		~SerialPort();

		// Communication
		unsigned	write(const void * srcBuffer, unsigned nBytes); // Returns the amount of bytes written
		void		write(u8); // Writes a single byte
		unsigned	read (void * dstBuffer, unsigned nBytes); // Returns the amount of bytes read
		u8			read (); // Reads one byte

		bool		isReady () const;

		// Callbacks
		// TODO

	private:
		// Disable copy
		// SerialPort(const SerialPort&);
		// SerialPort(SerialPort&&);
		// SerialPort& operator=(const SerialPort&);
		// SerialPort& operator=(SerialPort&&);
		DECLARE_COPY(SerialPort);

	private:
#ifdef WIN32
		// Windows specific members
		HANDLE	mPortHandle;
#endif // WIN32
		bool	mReady;
	};
	
	// --- Inline definitions -------------------------------------------
	bool SerialPort::isReady() const { return mReady; }

}	// namespace comm
}	// namespace rev

#endif // REV_ENABLE_SERIAL_PORT

#endif // _REV_COMM_SERIAL_SERIALPORT_H_
