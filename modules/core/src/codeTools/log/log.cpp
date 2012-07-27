//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Log system

#include "log.h"

#ifdef REV_ENABLE_LOG
#ifdef WIN32
#include <iostream>
#endif // WIN32

namespace rev { namespace codeTools
{
	//------------------------------------------------------------------------------------------------------------------
	// Global data
	const unsigned	gBufferLength = 
#ifdef WIN32
		1024;
#endif // WIN32
#ifdef ATMEGA
		4;
#endif // ATMEGA
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
		flushBuffer(mBuffer, mInternalCursor);
		resetBuffer();
	}

	//------------------------------------------------------------------------------------------------------------------
	Log::Log()
#ifdef WIN32
		:mBuffer(new char[gBufferLength+1])
#else
		:mBuffer(new char[gBufferLength])
#endif
		,mInternalCursor(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	Log::~Log()
	{
		flush();
		delete[] mBuffer;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Log::logString(const std::string& _s)
	{
#ifdef REV_BUFFER_LOG
		unsigned msgLength = _s.length();
		if(msgLength > (gBufferLength - mInternalCursor)) // No space available
		{
			flushBuffer(mBuffer, mInternalCursor);
			resetBuffer();
			if(msgLength > gBufferLength)
			{
				flushBuffer(_s.c_str(), msgLength);
				return;
			}
		}
		// Copy the message
		for(auto i = _s.begin(); i != _s.end(); ++i)
			mBuffer[mInternalCursor++] = *i;
#else // !REV_BUFFER_LOG
		flushBuffer(_s.c_str(), _s.length());
#endif // !REV_BUFFER_LOG
	}

	//------------------------------------------------------------------------------------------------------------------
	void Log::flushBuffer(const char * _buffer, unsigned _length)
	{
#ifdef WIN32
		_length; // Unused variable
		std::cout << _buffer;
#endif // WIN32
	}

#ifdef REV_BUFFER_LOG
	//------------------------------------------------------------------------------------------------------------------
	void Log::resetBuffer()
	{
		mInternalCursor = 0;
#ifdef WIN32
		for(unsigned i = 0; i < gBufferLength+1; ++i)
		{
			mBuffer[i] = 0;
		}
#endif // WIN32
	}
#endif // REV_BUFFER_LOG

}	// namespace codeTools
}	// namespace rev

#endif // REV_ENABLE_LOG
