////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script variant

#include "variant.h"

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/string.h>

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
		else if(mType == eString)
			mString = _x.mString;
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
	CVariant::CVariant(const char * _str)
		:mType(eString)
		,mString(_str)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant::CVariant(const string& _str)
		:mType(eString)
		,mString(_str)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant& CVariant::operator=(const CVariant& _x)
	{
		if(mType == eList)
			mList.clear();

		if((mType = _x.mType) == eList)
			mList = _x.mList;
		else if(mType == eString)
			mString = _x.mString;
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
	CVariant& CVariant::operator=(bool _x)
	{
		if(mType == eList)
			mList.clear();
		mType = eBool;
		mData.b = _x;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant& CVariant::operator=(const char * _x)
	{
		if(mType == eList)
			mList.clear();
		mType = eString;
		mString = string(_x);
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVariant& CVariant::operator=(const string& _x)
	{
		if(mType == eList)
			mList.clear();
		mType = eString;
		mString = _x;
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

	//------------------------------------------------------------------------------------------------------------------
	void CVariant::log(unsigned _indent)
	{
		// Indent the log
		string indent = "";
		for(unsigned tabs = 0; tabs<_indent; ++tabs)
			indent += ("  ");
		// Log content
		switch(mType)
		{
		case eList:
			codeTools::SLog::logN(indent + "[");
			for(rtl::vector<CVariant>::iterator element = mList.begin(); element != mList.end(); ++element)
			{
				element->log(_indent+1);
			}
			codeTools::SLog::logN(indent + "]");
			break;
		case eInteger:
			codeTools::SLog::log(indent);
			codeTools::SLog::logN(mData.i);
			break;
		case eBool:
			if(mData.b)
				codeTools::SLog::logN(indent + "true");
			else
				codeTools::SLog::logN(indent + "false");
			break;
		case eReal:
			codeTools::SLog::log(indent);
			codeTools::SLog::logN(mData.d);
			break;
		case eString:
			codeTools::SLog::logN(indent + mString);
			break;
		default:
				codeTools::SLog::logN(indent + "Log error, unkown Variant type");
		}
	}
}	// namespace rev
