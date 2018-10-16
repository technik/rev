#pragma once

#include <Windows.h>
#include <cstdint>

namespace dmc {

	class SerialWin32 {
	public:
		SerialWin32			(const char* _port, unsigned _baudRate);

		unsigned	write	(const void* _src, unsigned _nBytes);
		bool		write	(uint8_t);

		unsigned	read	(void * _dst, unsigned _nBytes); 
		uint8_t		read	(); 

	private:
		void openPort		(const char* _port);
		void setBaudRate	(unsigned _baudRate);

	private:
		HANDLE		mPortHandle;
	};

	typedef SerialWin32 SerialBase;

}	// namespace dmc