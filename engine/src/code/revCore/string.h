////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, types
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 10th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// strings

#ifndef _REV_REVCORE_STRING_H_
#define _REV_REVCORE_STRING_H_

// Standard headers
#if defined(_linux) || defined(ANDROID) || defined (WIN32)
#include <string>
#endif // _linux || ANDROID || WIN32

namespace rev
{

#if defined(_linux) || defined(ANDROID) || defined (WIN32)

using std::string;

unsigned	stringLength		(const char * _str);
bool		isCharacterInString	(char, const char * _str);
bool		compareString		(const char * _a, const char * _b);
bool		compareString		(const char * _a, const char * _b, unsigned _n);
void		copyString			(char * _dst, const char * _src);
void		copyStringN			(char * _dst, const char * _src, unsigned _maxLength);
int			integerFromString	(const char * _str);

#endif // _linux || ANDROID || WIN32

}	// namespace rev

#endif // _REV_REVCORE_STRING_H_