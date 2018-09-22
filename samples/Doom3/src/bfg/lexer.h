//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// A doom 3 map loader
#pragma once

#include <fstream>
#include <string>

namespace id
{
	struct Lexer
	{
		Lexer(const char *fileName)
		{
			std::ifstream inFile(fileName, std::ifstream::ate);
			if(!inFile.is_open())
				return;

			auto len = inFile.tellg();
			inFile.seekg(std::ios::beg);

			buffer.resize(len);
			inFile.read(buffer.data(), len);
		}

		bool empty() const {
			return ptr == buffer.size();
		}

		void skipLine()
		{
			auto lineEnd = buffer.find('\n', ptr);
			ptr = lineEnd+1;
		}

		void skipEmpty()
		{
			for(;;)
			{
				auto c = buffer[ptr];
				if(c == '/' && buffer[ptr] == '//') // Comment
					skipLine();
				if(c == '\n')
					++ptr;
			}
		}

		template<char c>
		bool readToken()
		{
			skipEmpty();
			if(buffer[ptr] == c)
			{
				++ptr;
				return true;
			}
			return false;
		}

		std::string buffer;
		int ptr = 0;
	};
}
