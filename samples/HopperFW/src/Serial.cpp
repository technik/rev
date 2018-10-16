#include <cassert>
#include "serial.h"

namespace dmc {

	//------------------------------------------------------------------------------------------------------------------
	SerialWin32::SerialWin32(const char* _port, unsigned _baudRate){
		// Enforce correct data
		assert(nullptr != _port && '\0' != _port[0]);

		openPort(_port);
		setBaudRate(_baudRate);
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned SerialWin32::write(const void* _src, unsigned _nBytes) {
		// Enforce correct input data
		assert(nullptr != _src);

		DWORD writtenBytes;
		::WriteFile(mPortHandle, _src, _nBytes, &writtenBytes, NULL);
		return writtenBytes;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned SerialWin32::read(void * _dstBuffer, unsigned _nBytes)
	{
		DWORD readBytes;
		::ReadFile(mPortHandle, _dstBuffer, _nBytes, &readBytes, NULL);
		return readBytes;

	}

	//------------------------------------------------------------------------------------------------------------------
	uint8_t SerialWin32::read()
	{
		uint8_t result;
		DWORD readBytes;
		::ReadFile(mPortHandle, &result, 1, &readBytes, NULL);
		return result;
	}
	//------------------------------------------------------------------------------------------------------------------
	bool SerialWin32::write(uint8_t _data) {
		DWORD writtenBytes;
		::WriteFile(mPortHandle, &_data, 1, &writtenBytes, NULL);
		return (0 != writtenBytes);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SerialWin32::openPort(const char* _port) {	
		mPortHandle = ::CreateFileA(_port,
			GENERIC_READ|GENERIC_WRITE,
			0, // Don't share the port
			0, // No security attributes
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0); // No templates
		assert(INVALID_HANDLE_VALUE != mPortHandle);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SerialWin32::setBaudRate(unsigned _baudRate)
	{
		DCB dcb = {0};
		dcb.DCBlength = sizeof(DCB);
		if (!::GetCommState (mPortHandle,&dcb))
			assert(false);
		switch(_baudRate) {
			case 4800:	dcb.BaudRate    = CBR_4800;		break;
			case 9600:	dcb.BaudRate    = CBR_9600;		break;
			case 19200:	dcb.BaudRate    = CBR_19200;	break;
			case 57600:	dcb.BaudRate    = CBR_57600;	break;
			case 115200:dcb.BaudRate    = CBR_115200;	break;
			case 128000:dcb.BaudRate    = CBR_128000;	break;
			case 256000:dcb.BaudRate    = CBR_256000;	break;
			default:
				assert(false); // Unsupported baudrate
				return;
		}
		dcb.ByteSize = 8;
		dcb.fParity = FALSE;
		dcb.Parity = 0;
		dcb.StopBits = 0;

		if (!::SetCommState (mPortHandle,&dcb))
			assert(false);
	}

}	// namespace dmc