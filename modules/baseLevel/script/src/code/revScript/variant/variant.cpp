//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 19th, 2013
//-------------------------------------------------------------------------
// Variant: Dynamic type data holder

#include "variant.h"

namespace rev { namespace script {

	//---------------------------------------------------------------------
	Variant::Variant(const Variant& _v)
		:mDataType(_v.mDataType)
	{
		mData = _v.mData;
	}

	//---------------------------------------------------------------------
	Variant& Variant::operator=(const Variant& _v)
	{
		mDataType = _v.mDataType;
		mData = _v.mData;
		return *this;
	}

	//---------------------------------------------------------------------
	Variant& Variant::operator=(int _x)
	{
		mDataType = Type::integer;
		mData.mInteger = _x;
		return *this;
	}

	//---------------------------------------------------------------------
	Variant& Variant::operator=(float _x)
	{
		mDataType = Type::real;
		mData.mReal = _x;
		return *this;
	}

}	// namespace script
}	// namespace rev