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

	__forceinline void CheckError(bool condition, std::string_view message)
	{
		if (!condition)
		{
			std::cout << "Error: " << message << std::endl;
			assert(false);
		}
	}

	__forceinline void LogMessage(std::string_view message)
	{
		std::cout << message << std::endl;
	}

}}	// namespace rev::core