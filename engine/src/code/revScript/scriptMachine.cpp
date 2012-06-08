////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on June 4th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#include "scriptMachine.h"

namespace rev { namespace script
{
	//------------------------------------------------------------------------------------------------------------------
	CScriptMachine * CScriptMachine::sInstance = 0;

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::init()
	{
		sInstance = new CScriptMachine();
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::end()
	{
		delete sInstance;
		sInstance = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	CScriptMachine * CScriptMachine::get()
	{
		return sInstance;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::createContextLevel()
	{
		mContextStack.resize(mContextStack.size()+1);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::destroyContextLevel()
	{
		TCtxLevel& level = mContextStack.back();
		for(TCtxLevel::iterator i = level.begin(); i != level.end(); ++i)
		{
			--mVarTable[i->second].first;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CScriptMachine::getVar(const char * _name)
	{
		// Check if the var exists
		TCtxLevel& level = mContextStack.back();
		TCtxLevel::iterator i = level.find(_name);
		if(i == level.end())
		{
			// If it doesn't exist, create it
			unsigned varIdx = getFirstEmptyVar();
			mContextStack.back().insert(string(_name), varIdx);
			return varIdx;
		}
		else
			return i->second;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::setVar(const char * _name, unsigned _idx)
	{
		TCtxLevel& level = mContextStack.back();
		TCtxLevel::iterator i = level.find(_name);
		if(i == level.end())
		{
			// If it doesn't exist, create it
			mContextStack.back().insert(string(_name), _idx);
		}
		else
			i->second = _idx;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CScriptMachine::addvar(CVariant& _v)
	{
		unsigned varIdx = getFirstEmptyVar();
		mVarTable[varIdx].first = 1;
		mVarTable[varIdx].second = _v;
		return varIdx;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::getValue(unsigned _idx, CVariant& _v)
	{
		_v = mVarTable[_idx].second;
		if(_v.type() == CVariant::eList)
		{
			for(unsigned i = 0; i < _v.size(); ++i)
			{
				CVariant element;
				getValue(_v[i].asInt(), element);
				_v[i] = element;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::getValue(const char * _name, CVariant& _v)
	{
		getValue(getVar(_name), _v);
	}

	//------------------------------------------------------------------------------------------------------------------
//	void CScriptMachine::assign(const char * _dst, const char * _src)
//	{
//		// Get source
//		unsigned dstIdx = getVar(_dst);
//		--mVarTable[dstIdx].first;
//		unsigned srcIdx = getVar(_src);
//		mContextStack.back()[_dst] = srcIdx;
//		++mVarTable[srcIdx].first;
//	}

	//------------------------------------------------------------------------------------------------------------------
	CScriptMachine::CScriptMachine()
	{
		mContextStack.resize(1);
	}

	//------------------------------------------------------------------------------------------------------------------
	CScriptMachine::~CScriptMachine()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CScriptMachine::getFirstEmptyVar()
	{
		for(unsigned i = 0; i < mVarTable.size(); ++i)
		{
			if(0 == mVarTable[i].first)
				return i;
		}
		unsigned size = mVarTable.size();
		mVarTable.resize(size+1);
		return size;
	}

}	// namespace script
}	// namespace rev