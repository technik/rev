////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script variant

#include "variant.h"

#include <revCore/codeTools/assert/assert.h>

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	CVariant::CVariant()
		:mType(eNill)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant::CVariant(const CVariant& _x)
		:mData(_x.mData)
	{
		if((mType = _x.mType) == eList)
			mList = _x.mList;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant::CVariant(int _i)
		:mType(eInteger)
	{
		mData.i = _i;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant::CVariant(double _d)
		:mType(eReal)
	{
		mData.d = _d;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant::CVariant(bool _b)
		:mType(eBool)
	{
		mData.b = _b;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant& CVariant::operator=(const CVariant& _x)
	{
		if(mType == eList)
			mList.clear();

		if((mType = _x.mType) == eList)
			mList = _x.mList;
		else
			mData = _x.mData;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant& CVariant::operator=(int _x)
	{
		if(mType == eList)
			mList.clear();
		mType = eInteger;
		mData.i = _x;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVariant::setNill()
	{
		if(mType == eList)
			mList.clear();
		mType = eNill;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVariant::append(const CVariant& _x)
	{
		// In case this is not a list, create a list from this
		if(mType != eList)
		{
			if(mType != eNill)
				mList.push_back(*this);
			mType = eList;
		}
		// Add the new element
		mList.push_back(_x);
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant& CVariant::operator[](unsigned _idx)
	{
		codeTools::revAssert((mType == eList) && (_idx < mList.size()));
		return mList[_idx];
	}
}	// namespace rev
