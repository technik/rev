////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on June 4th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#include "scriptMachine.h"
#include <revCore/codeTools/log/log.h>

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
	void CScriptMachine::addFunction(TScriptFunction _fn, const char * _name)
	{
		mCoreFunctions[_name] = _fn;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CScriptMachine::callFunction(const char * _name, const rtl::vector<unsigned>& _args)
	{
		if(mCoreFunctions.contains(_name))
		{
			CVariant arguments;
			// Gather raw argument data from the table of variables
			for(unsigned i = 0; i < _args.size(); ++i)
			{
				CVariant element;
				arguments.append(element);
				mData.getLiteral(_args[i], arguments[i]);
			}
			CVariant retValues; // To store actual return values
			if(mCoreFunctions[_name](arguments, retValues)) // If the function returns any value
			{
				unsigned retIdx = mData.newVar();
				mData.setLiteral(retIdx, retValues);
				return retIdx;
			}
		}
		else
		{
			revLog("Error [Script]: Attempt to call unknown function \"", eError);
			revLog(_name, eError);
			revLogN("\"", eError);
		}
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::getVar(const char * _name, CVariant& _dst)
	{
		if(mContext.contains(_name)) // if the variable exists in the current context
		{
			mData.getLiteral(mContext[_name], _dst);
		}
		else // else create a new empty variable
		{
			mContext[_name] = mData.newVar();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::setVar(const char * _name, const CVariant& _value)
	{
		unsigned varIdx;
		if(mContext.contains(_name)) // The variable already exists
		{
			varIdx = mContext[_name];
		}
		else
		{
			varIdx = mData.newVar();
			mContext[_name] = varIdx;
		}
		mData.setLiteral(varIdx, _value);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::setVar(const char * _name, unsigned _idx)
	{
		if(mContext.contains(_name)) // The variable already exists
		{
			unsigned& varIdx = mContext[_name];
			mData.lockVar(_idx); // It's important to lock before releasing because otherways trying to self-assign
			mData.releaseVar(varIdx); // variable could lead to reference corruption
			varIdx = _idx;
		}
		else
		{
			mContext[_name] = _idx;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScriptMachine::setVar(const char * _name, const rtl::vector<unsigned>& _indices)
	{
		CVariant indices;
		for(unsigned i = 0; i < _indices.size(); ++i)
		{
			indices.append(CVariant(int(_indices[i])));
		}
		unsigned idx = mData.newVar();
		mData.setRawValue(idx, indices);
		mContext[_name] = idx;
	}
	
	//------------------------------------------------------------------------------------------------------------------
	unsigned CScriptMachine::addData(const CVariant& _data)
	{
		unsigned i = mData.newVar();
		mData.setLiteral(i, _data);
		return i;
	}
	
	//------------------------------------------------------------------------------------------------------------------
	unsigned CScriptMachine::addData(const rtl::vector<unsigned>& _indices)
	{
		CVariant indices;
		for(unsigned i = 0; i < _indices.size(); ++i)
		{
			indices.append(CVariant(int(_indices[i])));
		}
		unsigned idx = mData.newVar();
		mData.setRawValue(idx, indices);
		return idx;
	}
	
	//------------------------------------------------------------------------------------------------------------------
	unsigned CScriptMachine::findData(const char * _name)
	{
		if(mContext.contains(_name))
			return mContext[_name];
		else
			return 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	CScriptMachine::CScriptMachine()
	{
		mData.newVar(); // Create a nill variable at index 0
	}

	//------------------------------------------------------------------------------------------------------------------
	CScriptMachine::~CScriptMachine()
	{
	}

}	// namespace script
}	// namespace rev