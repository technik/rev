//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Log system

#ifndef _REV_CORE_CODETOOLS_LOG_LOG_H_
#define _REV_CORE_CODETOOLS_LOG_LOG_H_

#include <codeTools/usefulMacros.h>

#ifdef REV_ENABLE_LOG
#include <string>
#endif // REV_ENABLE_LOG

namespace rev { namespace codeTools
{
	class Log
	{
	public:
		// Singleton management
		static void init();
		static void end();
		static Log& get();

		// Log interface
		template<class t_>
		Log& operator<< (const t_&);

		void flush();

	private:
		// Construction, destruction and copy
		Log();
		~Log();
		DECLARE_COPY(Log);

	private:
		void LogString(const std::string&);

	private:
#ifdef REV_ENABLE_LOG // Prevent the instance to occupy any space when Log is disabled
		static Log* sInstance;
#endif // REV_ENABLE_LOG

		char * mBuffer;
		char * mInternalCursor;
		char * mBufferSize;
	};

	
	template<class t_>
	Log& Log::operator<<(const t_& _message)
	{
#ifdef REV_ENABLE_LOG
		logString(makeString(_message));
#else // !REV_ENABLE_LOG
		_message; // Unused variable
#endif // !REV_ENABLE_LOG
		return *this;
	}

}	// namespace codeTools

	codeTools::Log&	revLog()
	{
		return codeTools::Log::get();
	}

}	// namespace rev

#endif // _REV_CORE_CODETOOLS_LOG_LOG_H_
