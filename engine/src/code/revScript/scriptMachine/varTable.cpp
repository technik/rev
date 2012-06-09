////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on June 9th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine's variables table

#include "varTable.h"

#include <revCore/codeTools/assert/assert.h>

namespace rev { namespace script
{
	//------------------------------------------------------------------------------------------------------------------
	void CVarTable::lockVar(unsigned _idx)
	{
		// Ensure valid index
		revAssert(_idx < mTable.size(), "Error[CVarTable]: variable index is out of range");
		// Increase var references
		++mTable[_idx].first;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVarTable::releaseVar(unsigned _idx)
	{
		// Ensure valid index
		revAssert(_idx < mTable.size(), "Error[CVarTable]: variable index is out of range");
		// Ensure the var is referenced
		revAssert(0 != mTable[_idx].first, "Error[CVarTable]: trying to release unreferenced variable");
		// Reduce var references
		--mTable[_idx].first;
		if(0 == mTable[_idx].first)
		{
			clearSlot(_idx);
			if(_idx < mFirstFreeSlot)
				mFirstFreeSlot = _idx;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CVarTable::newVar()
	{
		unsigned idx = getFirstEmptySlot();	// Locate an empty slot
		mTable[idx].first = 1;	// And add a reference to it
		return idx;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVarTable::getRawValue(unsigned _idx, CVariant& _dst) const
	{
		// Ensure valid index
		revAssert(_idx < mTable.size(), "Error[CVarTable]: variable index is out of range");
		// Retrieve the stored value directly, without performing any substitutions
		_dst = mTable[_idx].second;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVarTable::setRawValue(unsigned _idx, const CVariant& _value)
	{
		// Ensure valid index
		revAssert(_idx < mTable.size(), "Error[CVarTable]: variable index is out of range");
		// Clear any previous value
		clearSlot(_idx);
		// Directly store the raw value in the selected slot
		mTable[_idx].second = _value;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVarTable::getLiteral(unsigned _idx, CVariant& _dst) const
	{
		// Ensure valid index
		revAssert(_idx < mTable.size(), "Error[CVarTable]: variable index is out of range");
		// Clear destination variable
		_dst.setNill();
		// Locate the variable
		const CVariant& value = mTable[_idx].second;
		// if there's a table stored, perform substitution of indirect variables
		if(value.type() == CVariant::eList)
		{
			// Parse the list and retrieve indirect values from the table
			for(unsigned i = 0; i < value.size(); ++i)
			{
				// Add a new empty element to the result
				CVariant element;
				_dst.append(element);
				// Fill the new element with the referenced literal
				getLiteral(value[i].asInt(), _dst[i]);
			}
		}
		// else just copy the value
		else
		{
			_dst = value;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVarTable::setLiteral(unsigned _idx, const CVariant& _value)
	{
		// Ensure valid index
		revAssert(_idx < mTable.size(), "Error[CVarTable]: variable index is out of range");
		// Clear any previous value
		clearSlot(_idx);
		// Locate destination variable
		CVariant& value = mTable[_idx].second;
		// if the literal is a vector, store each element in an independent variable and keep the references as raw value
		if(_value.type() == CVariant::eList)
		{
			for(unsigned i = 0; i < _value.size(); ++i)
			{
				unsigned elementIdx = newVar();
				setLiteral(elementIdx, _value[i]);
				value.append(CVariant(int(elementIdx)));
			}
		}
		else // Else just store its value
		{
			value = _value;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CVarTable::clearSlot(unsigned _idx)
	{
		// Locate the variable
		CVariant& value = mTable[_idx].second;
		// If the slot contains a list of indirect references, release them
		if(value.type() == CVariant::eList)
		{
			for(unsigned i = 0; i < value.size(); ++i)
			{
				releaseVar(value[i].asInt());
			}
		}
		// Actually clear the value
		value.setNill();
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CVarTable::getFirstEmptySlot()
	{
		// mFirstFreeSlot always points to the first empty slot in the table. If the table is full, then it points just
		// after the end of the table, meaning we need to increase table size if we want to store more variables.
		if(mFirstFreeSlot == mTable.size())
		{
			mTable.resize(mTable.size() + 1); // Increase table size
			return mFirstFreeSlot++; // Increase it after return, so it keeps pointing to the end of the table
		}
		else
		{
			unsigned firstFree = mFirstFreeSlot;
			mFirstFreeSlot = nextFreeSlot(mFirstFreeSlot);
			return firstFree;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CVarTable::nextFreeSlot(unsigned _idx)
	{
		++_idx;
		while(_idx < mTable.size())
		{
			if(0 == mTable[_idx].first) // If the slot has no references, it means it's empty
			{
				// Found an empty slot, return it!
				return _idx;
			}
		}
		return _idx;
	}

}	// namespace script
}	// namespace rev
