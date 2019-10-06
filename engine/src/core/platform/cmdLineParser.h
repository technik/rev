//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once

#include <functional>
#include <string>
#include <sstream>
#include <string_view>
#include <unordered_map>

namespace rev::core
{
	class CmdLineParser
	{
	public:
		using FlagDelegate = std::function<void()>;
		using SimpleDelegate = std::function<void(const char*)>; // Will receive the first argument after the tag


		void addFlag(std::string_view tag, const FlagDelegate& _delegate);
		void addFlag(std::string_view tag, bool& dst);
		// Add an argument with just one parameter
		void addSimpleArgument(std::string_view tag, const SimpleDelegate& _delegate);

		template<class T>
		void addOption(std::string_view tag, T* dst);

		void parse(int argc, const char** argv);

	private:
		std::unordered_map<std::string, FlagDelegate> m_flags;
		std::unordered_map<std::string, SimpleDelegate> m_simpleArguments;
	};

	template<class T>
	inline void CmdLineParser::addOption(std::string_view tag, T* dst)
	{
		addSimpleArgument(tag, [=](const char* value) {
			std::stringstream ss;
			ss << value;
			ss >> *dst;
			});
	}

	// Partial specialization for std::string
	template<>
	inline void CmdLineParser::addOption(std::string_view tag, std::string* dst)
	{
		addSimpleArgument(tag, [=](const char* value) {
			*dst = value;
			});
	}
}