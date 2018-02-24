//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <string>
#include <iostream>
#ifdef ANDROID
#include <android/log.h>
#include <sstream>
#endif

namespace rev { namespace core {

	class Log {
	public:
		template<class ... Args_>
		static void verbose(Args_ ... _args)
		{
#ifdef _WIN32
			writeToLog("Verbose", _args ...);
#elif defined(ANDROID)
			writeToLog(ANDROID_LOG_VERBOSE, _args ...);
#endif
		}

		template<class ... Args_>
		static void debug(Args_ ... _args)
		{
#ifdef _WIN32
			writeToLog("Debug", _args ...);
#elif defined(ANDROID)
			writeToLog(ANDROID_LOG_DEBUG, _args ...);
#endif
		}

		template<class ... Args_>
		static void warning(Args_ ... _args)
		{
#ifdef _WIN32
			writeToLog("Info", _args ...);
#elif defined(ANDROID)
			writeToLog(ANDROID_LOG_WARN, _args ...);
#endif
		}

		template<class ... Args_>
		static void info(Args_ ... _args)
		{
#ifdef _WIN32
			writeToLog("Info", _args ...);
#elif defined(ANDROID)
			writeToLog(ANDROID_LOG_INFO, _args ...);
#endif
		}

		template<class ... Args_>
		static void error(Args_ ... _args)
		{
#ifdef _WIN32
			writeToLog("Info", _args ...);
#elif defined(ANDROID)
			writeToLog(ANDROID_LOG_ERROR, _args ...);
#endif
		}

	private:
#ifdef ANDROID
		template<typename ... Args_>
		static void writeToLog(android_LogPriority priority, Args_ ... _args)
		{
			std::stringstream ss;
			writeToSS(ss, _args...);
			__android_log_print(priority, "rev", "%s", ss.str().c_str());
		}
#else
		template<typename ... Args_>
		static void writeToLog(const char* _tag, Args_ ... _args)
		{
			std::cout << _tag << ": ";
			writeToSS(std::cout, _args...);
		}
#endif

		template<class T, typename ... Args_>
		static void writeToSS(std::ostream& ss, T x, Args_ ... _args)
		{
			ss << x;
			writeToSS(ss, _args...);
		}

		template<typename T>
		static void writeToSS(std::ostream& ss, T x)
		{
			ss << x;
		}
	};

}}	// namespace rev::core