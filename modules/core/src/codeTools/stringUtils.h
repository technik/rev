//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// String utils

#ifndef _REV_CORE_CODETOOLS_STRINGUTILS_H_
#define _REV_CORE_CODETOOLS_STRINGUTILS_H_

// External headers
#include <sstream>
#include <string>

namespace rev
{
	template<class t_>
	std::string	makeString(const t_& _x)
	{
		std::stringstream ss ( std::stringstream::out );
		ss << _x;
		return ss.str();
	}
}	// namespace rev

#endif // _REV_CORE_CODETOOLS_STRINGUTILS_H_
