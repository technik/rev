//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Serial port communications

// External headers
#ifdef WIN32
#include <Windows.h>
#include <string>
#endif // WIN32
#ifdef USE_ARDUINO_LIBS
#include <Arduino.h>
#endif // USE_ARDUINO_LIBS

// Rev headers
#include "serialPort.h"
#include <codeTools/log/log.h>

#ifdef REV_ENABLE_SERIAL_PORT

namespace rev { namespace comm
{
	//------------------------------------------------------------------------------------------------------------------
	SerialPort::SerialPort(uint8_t _port, uint32_t _baudRate, uint8_t _parityBits, uint8_t _stopBits)
		:mReady(false)
#ifdef WIN32
		,mPortHandle(INVALID_HANDLE_VALUE)
#endif // WIN32
	{
#ifdef WIN32
		// Get port name
		std::string portName = "COM0";
		portName[3] += _port;
		// Create the port
		mPortHandle = ::CreateFileA(portName.c_str(),
			GENERIC_READ|GENERIC_WRITE,
			0, // Don't share the port
			0, // No security attributes
			OPEN_EXISTING,
			FILE_ATTRIBUTE_NORMAL,
			0); // No templates
		if(mPortHandle == INVALID_HANDLE_VALUE) // Error checking
		{
			revLog() << "Unable to start " << portName << " port\n";
			DWORD error = GetLastError();
			if(error == ERROR_ACCESS_DENIED)
				revLog() << "Error: Access denied. Check the port isn't in use by any other application or try another port\n";
			else if(error == ERROR_FILE_NOT_FOUND)
				revLog() << "Error: Couldn't find " << portName << " port\n";
			else
				revLog() << "Error: Unknown error\n";
			revLog().flush();
			return;
		}
		// Configurate the port
		DCB dcb = {0};
		dcb.DCBlength = sizeof(DCB);
		if (!::GetCommState (mPortHandle,&dcb))
		{
			(revLog() << "Error: Failed to Get Comm State\n").flush();
			return;
		}
		switch(_baudRate)
		{
		case 4800:
			dcb.BaudRate    = CBR_4800;
			break;
		case 9600:
			dcb.BaudRate    = CBR_9600;
			break;
		case 19200:
			dcb.BaudRate    = CBR_19200;
			break;
		case 57600:
			dcb.BaudRate    = CBR_57600;
			break;
		case 115200:
			dcb.BaudRate    = CBR_115200;
			break;
		case 128000:
			dcb.BaudRate    = CBR_128000;
			break;
		case 256000:
			dcb.BaudRate    = CBR_256000;
			break;
		default:
			{
				(revLog() << "Error: " << _baudRate << " isn't a supported baud rate\n").flush();
				return;
			}
		}
		dcb.ByteSize = 8;
		dcb.fParity = FALSE;
		dcb.Parity = 0;
		if(_parityBits != 0)
			revLog() << "Warning: Parity bits are not supported\n Defaulting to no parity\n";
		if(_stopBits > 2)
		{
			revLog() << "Warning: Maximun stop bits is 2. Defaulting to 1\n";
			dcb.StopBits = 1;
		}
		else
			dcb.StopBits = _stopBits;

		if (!::SetCommState (mPortHandle,&dcb))
		{
			(revLog() << "Error: Failed to Set Comm State\n").flush();
		}
#endif // WIN32
#ifdef USE_ARDUINO_LIBS
		// Unsigned variables
		_parityBits;
		_stopBits;
		// Initialize the port
		switch(_port)
		{
		case 1:
			Serial1.begin(_baudRate);
			break;
		case 2:
			Serial2.begin(_baudRate);
			break;
		case 3:
			Serial3.begin(_baudRate);
			break;
		default:
			Serial.begin(_baudRate);
			break;
		}
#endif // USE_ARDUINO_LIBS
		mReady = true;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned SerialPort::write(const void * _srcBuffer, unsigned _nBytes)
	{
#ifdef WIN32
		DWORD writtenBytes;
		::WriteFile(mPortHandle, _srcBuffer, _nBytes, &writtenBytes, NULL);
		return writtenBytes;
#endif // WIN32
#ifdef USE_ARDUINO_LIBS
		// TODO
#endif // USE_ARDUINO_LIBS
	}

	//------------------------------------------------------------------------------------------------------------------
	void SerialPort::write(uint8_t _data)
	{
#ifdef WIN32
		DWORD writtenBytes;
		::WriteFile(mPortHandle, &_data, 1, &writtenBytes, NULL);
#endif // WIN32
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned SerialPort::read(void * _dstBuffer, unsigned _nBytes)
	{
#ifdef WIN32
		DWORD readBytes;
		::ReadFile(mPortHandle, _dstBuffer, _nBytes, &readBytes, NULL);
		return readBytes;
#endif // WIN32
	}

	//------------------------------------------------------------------------------------------------------------------
	uint8_t SerialPort::read()
	{
#ifdef WIN32
		uint8_t result;
		DWORD readBytes;
		::ReadFile(mPortHandle, &result, 1, &readBytes, NULL);
		return result;
#endif // WIN32
	}
}	// namespace comm
}	// namespace rev

#endif // REV_ENABLE_SERIAL_PORT
