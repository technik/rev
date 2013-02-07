//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Log system

#include "log.h"

#if !(defined( ATMEGA ) || defined(ANDROID))
#include <iostream>
#endif // !ATMEGA, !ANDROID

namespace rev { namespace codeTools
{
	// Static data
	Log* Log::sInstance = nullptr;

	//------------------------------------------------------------------------------------------------------------------
	void Log::init()
	{
#ifdef REV_ENABLE_LOG
		sInstance = new Log();
#else
		sInstance = nullptr;
#endif
	}

	//------------------------------------------------------------------------------------------------------------------
	void Log::end()
	{
		delete sInstance;
		sInstance = nullptr;
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
	void Log::logBuffer(const char* _s)
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
		flushBuffer(_s);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Log::flushBuffer(const char * _buffer)
	{
#if !(defined( ATMEGA ) || defined(ANDROID))
		// We assume 0 terminated buffers
		std::cout << _buffer;
#endif // !ATMEGA, !ANDROID
	}

}	// namespace codeTools
}	// namespace rev
