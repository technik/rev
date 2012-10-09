//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Ag�era Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Log system

#include "log.h"

#ifndef ATMEGA
#include <iostream>
#endif // !ATMEGA

namespace rev { namespace codeTools
{
	// Static data
	Log* Log::sInstance = nullptr;

	//------------------------------------------------------------------------------------------------------------------
	void Log::init()
	{
		sInstance = new Log();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Log::end()
	{
		delete sInstance;
		sInstance = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Log::flush()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	Log::Log()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	Log::~Log()
	{
		flush();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Log::logString(const std::string& _s)
	{
		// unsigned msgLength = _s.length();
		// if(msgLength > (gBufferLength - mInternalCursor)) // No space available
		// {
		// 	flushBuffer(mBuffer, mInternalCursor);
		// 	resetBuffer();
		// 	if(msgLength > gBufferLength)
		// 	{
		// 		flushBuffer(_s.c_str(), msgLength);
		// 		return;
		// 	}
		// }
		// // Copy the message
		// for(auto i = _s.begin(); i != _s.end(); ++i)
		// 	mBuffer[mInternalCursor++] = *i;
		flushBuffer(_s.c_str());
	}

	//------------------------------------------------------------------------------------------------------------------
	void Log::flushBuffer(const char * _buffer)
	{
#ifndef ATMEGA
		// We assume 0 terminated buffers
		std::cout << _buffer;
#endif // !ATMEGA
	}

}	// namespace codeTools
}	// namespace rev
