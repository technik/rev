//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On November 21st, 2012
//----------------------------------------------------------------------------------------------------------------------
// Arduino link's communication package

#ifndef _ARDUINOLINK_COMMUNICATION_PACKAGE_PACKAGE_H_
#define _ARDUINOLINK_COMMUNICATION_PACKAGE_PACKAGE_H_

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
	Package		(); // Empty package construction
	Package		(Type _type, uint8_t _length, const void* _data); // Build a package using data.
	Package		(const Package&); // Copy constructor
	Package		(Package&&); // Move constructor
	~Package	();

	Package& operator= (const Package&);
	Package& operator= (Package&&);

	// Data
	void			setSize(uint8_t _size); // Sets size and allocates space for data
	void			addData(uint8_t _byte);	// Assumes enough data is available in the packet
	
	// Accesors
	Type			type	() const;
	uint8_t			dataSize() const;
	const uint8_t*	data	() const;

	uint16_t		checksum() const;

private:
	uint8_t*	mData;
	uint16_t	mChecksum;
	uint8_t		mDataSize;
	uint8_t		mDataIdx;
	Type		mType;
};

//-------------------------------------------------------------------------------------------------------------------
// Inline implementations

//-------------------------------------------------------------------------------------------------------------------
inline Package::Package()
	:mData(nullptr)
	,mChecksum(0)
	,mDataSize(0)
	,mDataIdx(0)
{
}

//-------------------------------------------------------------------------------------------------------------------
inline Package::Package(Package&& _x)
	:mData(_x.mData)
	,mChecksum(_x.mChecksum)
	,mDataSize(_x.mDataSize)
	,mDataIdx(_x.mDataIdx)
	,mType(_x.mType)
{
	_x.mData = nullptr;
}

//-------------------------------------------------------------------------------------------------------------------
inline Package::Type Package::type() const
{
	return mType;
}

//-------------------------------------------------------------------------------------------------------------------
inline uint8_t Package::dataSize() const
{
	return mDataSize;
}

//-------------------------------------------------------------------------------------------------------------------
inline const uint8_t* Package::data() const
{
	return mData;
}

//-------------------------------------------------------------------------------------------------------------------
inline uint16_t Package::checksum() const
{
	return mChecksum;
}

#endif // _ARDUINOLINK_COMMUNICATION_PACKAGE_PACKAGE_H_
