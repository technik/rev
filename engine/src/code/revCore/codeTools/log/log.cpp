////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, code tools, log
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on April 9th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Basic log interface

#include "log.h"

#include <revCore/codeTools/assert/assert.h>

#if defined(WIN32) || defined(_linux)
#include <iostream>
#endif // win32 || linux

namespace rev
{
	namespace codeTools
	{
		const unsigned bufferSize = 2048;
		//--------------------------------------------------------------------------------------------------------------
		SLog * SLog::sInstance = 0;

		//--------------------------------------------------------------------------------------------------------------
		void SLog::init()
		{
			revAssert( 0 == sInstance , "Error: SLog is already initialized-");
			sInstance = new SLog();
			revAssert( 0 != sInstance); // We can't set an error message if SLog doesn't exist at all
		}

		//--------------------------------------------------------------------------------------------------------------
		void SLog::end()
		{
			revAssert( 0 != sInstance );
			delete sInstance;
			sInstance = 0;
		}

		//--------------------------------------------------------------------------------------------------------------
		SLog * SLog::get()
		{
			revAssert(0 != sInstance);
			return sInstance;
		}

		//--------------------------------------------------------------------------------------------------------------
		void SLog::newLine(ELogChannel _channel)
		{
			log("\n", _channel);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<>
		void SLog::log(const char * _msg, ELogChannel _channel)
		{
			if(mEnableGlobal && mEnableChannel[_channel])
			{
				unsigned len = stringLength(_msg);
				if(len+1 > (bufferSize - mBufferCursor))
				{
					flush();
					if(len > bufferSize)
						flushString(_msg);
					else
					{
						copyString(&mBuffer[mBufferCursor], _msg);
						mBufferCursor += len;
					}
				}
				else
				{
					copyString(&mBuffer[mBufferCursor], _msg);
					mBufferCursor += len;
				}
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void SLog::flush()
		{
			flushString(mBuffer);
			resetBuffer();
		}

		//--------------------------------------------------------------------------------------------------------------
		void SLog::flushString(const char * _s)
		{
#ifdef WIN32
			std::cout << _s;
#endif // WIN32
		}

		//--------------------------------------------------------------------------------------------------------------
		void SLog::resetBuffer()
		{
			mBufferCursor = 0;
			mBuffer[0] = 0;
		}

		//--------------------------------------------------------------------------------------------------------------
		SLog::SLog()
			:mEnableGlobal(true)
			,mBufferCursor(0)
		{
			for(unsigned i = 0; i < eMaxLogChannel; ++i)
			{
				mEnableChannel[i] = true;
			}
			mBuffer = new char[bufferSize];
			resetBuffer();
		}

		//--------------------------------------------------------------------------------------------------------------
		SLog::~SLog()
		{
			flush(); // Flush pending messages so they don't get lost
			delete[] mBuffer;
		}
	}	// namespace codeTools

}	// namespace rev
