//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Ag�era Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Log system

#ifndef _REV_CORE_CODETOOLS_LOG_LOG_H_
#define _REV_CORE_CODETOOLS_LOG_LOG_H_

#include <codeTools/usefulMacros.h>

#ifdef REV_ENABLE_LOG
#include <string>
#include <codeTools/stringUtils.h>
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
		template<class T_>
		Log& operator<< (const T_&);
		void flush();

	private:
		// Construction, destruction and copy
		Log();
		~Log();
		DECLARE_COPY(Log); // Prevent copy

	private:
		virtual void	logString	(const std::string&);
		virtual void	flushBuffer	(const char*);

	private:
		static Log* sInstance;
	};

	// Inline implementations
	inline Log& Log::get()
	{
#ifdef REV_ENABLE_LOG
		return *sInstance;
#else // !REV_ENABLE_LOG
		return *((Log*)nullptr); // Return a reference to a null pointer
#endif // !REV_ENABLE_LOG
	}

	template<class T_>
	Log& Log::operator<<(const T_& _message)
	{
#ifdef REV_ENABLE_LOG
		logString(makeString(_message));
#else // !REV_ENABLE_LOG
		_message; // Unused variable
#endif // !REV_ENABLE_LOG
		return *this;
	}

}	// namespace codeTools

	inline codeTools::Log&	revLog()
	{
		return codeTools::Log::get();
	}

}	// namespace rev

#endif // _REV_CORE_CODETOOLS_LOG_LOG_H_
