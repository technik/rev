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
#include <sstream>
#endif // _linux || ANDROID || WIN32

namespace rev
{

#if defined(_linux) || defined(ANDROID) || defined (WIN32)
typedef std::string string;
#endif // linux || android || windows

unsigned	stringLength		(const char * _str);
bool		isCharacterInString	(char, const char * _str);
bool		compareString		(const char * _a, const char * _b);
bool		compareString		(const char * _a, const char * _b, unsigned _n);
void		copyString			(char * _dst, const char * _src);
void		copyStringN			(char * _dst, const char * _src, unsigned _maxLength);
int			integerFromString	(const char * _str);
double		doubleFromString	(const char * _str);

// Stringification
template < class T >
char*		makeString			( T _x )
{
#if defined(_linux) || defined(ANDROID) || defined (WIN32)
	std::stringstream ss ( std::stringstream::in || std::stringstream::out );
	ss << _x;
	string str = ss.str();
	const char * text = str.c_str();

	unsigned len = stringLength(text);
	char * retText = new char[len+1];
	copyString(retText, text);

	return retText;
#endif // linux || android || windows
}

}	// namespace rev

#endif // _REV_REVCORE_STRING_H_