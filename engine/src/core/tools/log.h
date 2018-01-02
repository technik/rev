//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <string>
#include <iostream>
#ifdef ANDROID
#include <android/log.h>
#endif

namespace rev { namespace core {

	class Log {
	public:
		static void verbose(const std::string& _s)
		{
#ifdef _WIN32
			std::cout << "V: " << _s << "\n";
#elif defined(ANDROID)
			__android_log_print(ANDROID_LOG_VERBOSE, "rev", "%s", _s.c_str());
#endif
		}
		static void debug(const std::string& _s)
		{
#ifdef _WIN32
			std::cout << "D: " << _s << "\n";
#elif defined(ANDROID)
			__android_log_print(ANDROID_LOG_DEBUG, "rev", "%s", _s.c_str());
#endif
		}
		static void warning(const std::string& _s)
		{
#ifdef _WIN32
			std::cout << "W: " << _s << "\n";
#elif defined(ANDROID)
			__android_log_print(ANDROID_LOG_WARN, "rev", "%s", _s.c_str());
#endif
		}
		static void info(const std::string& _s)
		{
#ifdef _WIN32
			std::cout << "I: " << _s << "\n";
#elif defined(ANDROID)
			__android_log_print(ANDROID_LOG_INFO, "rev", "%s", _s.c_str());
#endif
		}
		static void error(const std::string& _s)
		{
#ifdef _WIN32
			std::cout << "E: " << _s << "\n";
#elif defined(ANDROID)
			__android_log_print(ANDROID_LOG_ERROR, "rev", "%s", _s.c_str());
#endif
		}
	};

}}	// namespace rev::core