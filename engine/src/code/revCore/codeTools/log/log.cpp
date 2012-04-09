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
		//--------------------------------------------------------------------------------------------------------------
		SLog * SLog::sInstance = 0;

		//--------------------------------------------------------------------------------------------------------------
		void SLog::init()
		{
			codeTools::revAssert( 0 == sInstance );
			sInstance = new SLog();
			codeTools::revAssert( 0 != sInstance );
		}

		//--------------------------------------------------------------------------------------------------------------
		void SLog::end()
		{
			codeTools::revAssert( 0 != sInstance );
			delete sInstance;
			sInstance = 0;
		}

		//--------------------------------------------------------------------------------------------------------------
		SLog * SLog::get()
		{
			codeTools::revAssert(0 != sInstance);
			return sInstance;
		}

		//--------------------------------------------------------------------------------------------------------------
		void SLog::newLine(ELogChannel _channel)
		{
			log('\n', _channel);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<>
		void SLog::log(const char * _msg, ELogChannel _channel)
		{
			if(mEnableGlobal && mEnableChannel[_channel])
			{
				char * msg = new char[stringLength(_msg)+1];
				copyString(msg, _msg);
				mBuffer.push_back(msg);
				if(mBuffer.size() >= mBufferCapacity)
					flush();
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void SLog::flush()
		{
			for(unsigned i = 0; i < mBuffer.size(); ++i)
			{
				char * message = mBuffer[i];
#if defined(WIN32) || defined(_linux)
				std::cout << message;
#endif // win32 || linux
				delete[] message;
			}
			mBuffer.clear();
		}

		//--------------------------------------------------------------------------------------------------------------
		void SLog::setBufferCapacity(unsigned _capacity)
		{
			mBuffer.reserve(_capacity);
			mBufferCapacity = _capacity;
		}

		//--------------------------------------------------------------------------------------------------------------
		SLog::SLog()
			:mEnableGlobal(true)
			,mBufferCapacity(100)
		{
			for(unsigned i = 0; i < eMaxLogChannel; ++i)
			{
				mEnableChannel[i] = true;
			}
			mBuffer.reserve(mBufferCapacity);
		}

		//--------------------------------------------------------------------------------------------------------------
		SLog::~SLog()
		{
			flush(); // Flush pending messages so they don't get lost
		}
	}	// namespace codeTools

}	// namespace rev
