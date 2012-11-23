//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On November 22nd, 2012
//----------------------------------------------------------------------------------------------------------------------
// Arduino link's communication package

#include "package.h"

//----------------------------------------------------------------------------------------------------------------------
Package::Package(Type _type, uint8_t _length, const void* _data)
	:mType(_type)
	,mChecksum(0)
{
	setSize(_length);
	for(unsigned i = 0; i < _length; ++i)
		addData(reinterpret_cast<const uint8_t*>(_data)[i]);
}

//----------------------------------------------------------------------------------------------------------------------
Package::Package(const Package& _x)
	:mType(_x.mType)
	,mChecksum(0)
{
	setSize(_x.mDataSize);
	for(unsigned i = 0; i < _x.mDataSize; ++i)
		addData(_x.mData[i]);
}

//----------------------------------------------------------------------------------------------------------------------
Package::~Package()
{
	if(nullptr != mData)
		delete[] mData;
}

//----------------------------------------------------------------------------------------------------------------------
void Package::setSize(uint8_t _size)
{
	mDataIdx = 0;
	mDataSize = _size;
	mData = new uint8_t[_size];
}

//----------------------------------------------------------------------------------------------------------------------
void Package::addData(uint8_t _data)
{
	mData[mDataIdx++] = _data;
	mChecksum += _data;
}