//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 19th, 2013
//-------------------------------------------------------------------------
// Script Virtual Machine's data table

#include "dataTable.h"

namespace rev { namespace script {
	//---------------------------------------------------------------------
	unsigned DataTable::addData(const Variant& _x)
	{
		unsigned index = firstEmptyIndex();
		std::pair<Variant, unsigned>& freeCell = mTable[index];
		freeCell.second = 1;
		freeCell.first = _x;
		return index;
	}
	
	//---------------------------------------------------------------------
	void DataTable::consultData(unsigned _index, Variant& _dst) const
	{
		_dst = mTable[_index].first;
	}
	
	//---------------------------------------------------------------------
	void DataTable::setData(unsigned _index, const Variant& _data)
	{
		mTable[_index].first = _data;
	}

	//---------------------------------------------------------------------
	unsigned DataTable::firstEmptyIndex()
	{
		for(unsigned i = 0; i < mTable.size(); ++i)
		{
			if(mTable[i].second == 0)
				return i;
		}
		// No empty holes. Add a new one
		Variant dummy;
		mTable.push_back(std::make_pair(dummy, unsigned(0)));
		return mTable.size() - 1;
	}
}	// namespace script
}	// namespace rev