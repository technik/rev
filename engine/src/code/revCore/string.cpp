////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, types
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on April 1st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// strings
#ifndef _REV_CORE_STRING_H_
#define _REV_CORE_STRING_H_

namespace rev
{
	//----------------------------------------------------------------------------------------------------------------------
	unsigned	stringLength		(const char * _str)
	{
		unsigned i;
		for(i = 0; _str[i] != '\0'; ++i)
		{}
		return i;
	}

	//----------------------------------------------------------------------------------------------------------------------
	bool		isCharacterInString	(char _a, const char * _str)
	{
		for(unsigned i = 0; _str[i] != '\0'; ++i)
		{
			if(_a == _str[i])	// Found character
				return true;
		}
		// Not found
		return false;
	}

	//----------------------------------------------------------------------------------------------------------------------
	void		copyString			(char * _dst, const char * _src)
	{
		unsigned i = 0;
		for(; _src[i] != '\0'; ++i)
		{
			_dst[i] = _src[i];
		}
		_dst[i] = '\0';
	}

	//----------------------------------------------------------------------------------------------------------------------
	void		copyStringN			(char * _dst, const char * _src, unsigned _maxLength)
	{
		unsigned i = 0;
		for(; _src[i] != '\0', i < _maxLength; ++i)
		{
			_dst[i] = _src[i];
		}
		_dst[i] = '\0';
	}

	//----------------------------------------------------------------------------------------------------------------------
	int integerFromString(const char * _str)
	{
		unsigned cursor = 0;
		int integer = 0;
		char digit = _str[cursor];
		while(digit != '\0')
		{
			integer *= 10;	// Multiply previous result by ten
			integer += digit - '0';	// Add the value of the new digit
			// Go to the next digit
			digit = _str[++cursor];
		}
		return integer;
	}

	//----------------------------------------------------------------------------------------------------------------------
	double doubleFromString(const char * _str)
	{
		if(!isCharacterInString('.', _str))
		{
			return double(integerFromString(_str));
		}else
		{	
			unsigned cursor = 0;
			int integer = 0;
			char digit = _str[cursor];
			while(digit != '.')
			{
				integer *= 10;	// Multiply previous result by ten
				integer += digit - '0';	// Add the value of the new digit
				// Go to the next digit
				digit = _str[++cursor];
			}
			++cursor; // Skip period
			double decimal = double(integerFromString(&_str[cursor]));
			while(_str[cursor++] != '\0')
				decimal /= 10.0;
			return double(integer) + decimal;
		}
	}

	//----------------------------------------------------------------------------------------------------------------------
	bool compareString(const char * _a, const char * _b)
	{
		if (!_a || !_b)
			return false;
		unsigned i = 0;
		while(_a[i] != '\0' && _b[i] != '\0')
		{
			if(_a[i] != _b[i])
				return false;
			++i;
		}
		return _a[i] == _b[i];
	}

	//----------------------------------------------------------------------------------------------------------------------
	bool compareString(const char * _a, const char * _b, unsigned _n)
	{
		if (!_a || !_b)
			return false;
		unsigned i = 0;
		while(i < _n && _a[i] != '\0' && _b[i] != '\0')
		{
			if(_a[i] != _b[i])
				return false;
			++i;
		}
		return true;
	}
}	// namespace rev

#endif // _REV_CORE_STRING_H_
