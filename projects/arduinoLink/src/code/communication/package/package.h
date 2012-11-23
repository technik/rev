//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On November 21st, 2012
//----------------------------------------------------------------------------------------------------------------------
// Arduino link's communication package

#include <cstdint>

class Package
{
public:
	enum class Type
	{
		debug,
		data
	};
public:
	Package(); // Empty package construction
	Package(Type _type, uint8_t _length, const void* _data); // Build a package using data
	Package(const Package&); // Copy constructor
	Package(Package&&); // Move constructor
	~Package();
};