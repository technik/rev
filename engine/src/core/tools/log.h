//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <string>
#include <iostream>

namespace rev { namespace core {

	class Log {
	public:
		static void verbose(const std::string& _s)
		{
#ifdef _WIN32
			std::cout << "V: " << _s << "\n";
#elif defined(ANDROID)
#endif
		}
		static void debug(const std::string& _s)
		{
#ifdef _WIN32
			std::cout << "V: " << _s << "\n";
#elif defined(ANDROID)
#endif
		}
		static void warning(const std::string& _s)
		{
#ifdef _WIN32
			std::cout << "V: " << _s << "\n";
#elif defined(ANDROID)
#endif
		}
		static void info(const std::string& _s)
		{
#ifdef _WIN32
			std::cout << "V: " << _s << "\n";
#elif defined(ANDROID)
#endif
		}
		static void error(const std::string& _s)
		{
#ifdef _WIN32
			std::cout << "V: " << _s << "\n";
#elif defined(ANDROID)
#endif
		}
	};

}}	// namespace rev::core