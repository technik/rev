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

#ifdef REV_ENABLE_SERIAL_PORT

namespace rev { namespace comm
{
	//------------------------------------------------------------------------------------------------------------------
	SerialPort::SerialPort(unsigned _port, unsigned _baudRate, unsigned _parityBits, unsigned _stopBits)
		:mReady(false)
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
			return;
		// Configurate the port

		if(m_hCommPort != INVALID_HANDLE_VALUE)
		{
			serialOpened = true;
		}
		else
		{
			DWORD error = GetLastError();
			if(error == ERROR_ACCESS_DENIED)
			{
				cout << "Access denied to " << comPort << endl;
				cout << "Check the port isn't in use by any other application or try another port" << endl;
			}
			else if (error == ERROR_FILE_NOT_FOUND)
			{
				cout << "Couldn't find " << comPort << endl;
			}
			else
			{
				cout << "Unknown error" << endl;
			}
		}
	}

	DCB dcb = {0};
	dcb.DCBlength = sizeof(DCB);

	if (!::GetCommState (m_hCommPort,&dcb))
	{
		cout << "Failed to Get Comm State" << endl;
	}

	dcb.BaudRate    = CBR_115200;
	dcb.ByteSize    = 8;
	dcb.fParity		= FALSE;
	dcb.Parity      = 0;
	dcb.StopBits    = ONESTOPBIT;

	if (!::SetCommState (m_hCommPort,&dcb))
	{
		cout << "Failed to Set Comm State" << endl;
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
	}
}	// namespace comm
}	// namespace rev

#endif // REV_ENABLE_SERIAL_PORT
