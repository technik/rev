//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 19th, 2013
//-------------------------------------------------------------------------
// Script Virtual Machine

#include "scriptVM.h"
#include "backend/scriptVMBackend.h"

namespace rev { namespace script {

	//---------------------------------------------------------------------
	ScriptVM::ScriptVM()
	{
		mBackend = new ScriptVMBackend();
	}

	//---------------------------------------------------------------------
	ScriptVM::~ScriptVM()
	{
		delete mBackend;
	}

	//---------------------------------------------------------------------
	void ScriptVM::execute(const char* _code)
	{
		mBackend->buildExecTree(_code);
	}

	//---------------------------------------------------------------------
	void ScriptVM::setVar(const char* _name, const Variant& _data)
	{
		std::string name(_name);
		// Does the named variable already exist in the context?
		auto iterator = mVarTable.find(name);
		if(iterator != mVarTable.end()) { // Found the var
			unsigned index = iterator->second;
			mStoredData.setData(index, _data);
		} else { // Variable not found
			unsigned index = mStoredData.addData(_data);
			mVarTable[name] = index;
		}
	}

	//---------------------------------------------------------------------
	void ScriptVM::getVar(const char* _name, Variant& _data)
	{
		std::string name(_name);
		// Does the named variable already exist in the context?
		auto iterator = mVarTable.find(name);
		if(iterator != mVarTable.end()) { // Found the var
			unsigned index = iterator->second;
			mStoredData.consultData(index, _data);
		} else { // Variable not found
			_data.setNill(); // Nill the output
			// Create an empty entry in the data table
			unsigned index = mStoredData.addData(_data);
			mVarTable[name] = index;
		}
	}

}	// namespace script
}	// namespace rev