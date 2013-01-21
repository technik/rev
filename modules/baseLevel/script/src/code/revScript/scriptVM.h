//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 18th, 2013
//-------------------------------------------------------------------------
// Script Virtual Machine

#ifndef _REV_SCRIPT_SCRIPTVM_H_
#define _REV_SCRIPT_SCRIPTVM_H_

#include <string>
#include <unordered_map>
#include "virtualMachine/dataTable.h"

namespace rev { namespace script
{
	// Forward declarations
	class Variant;
	class ScriptVMBackend;

	//--------------------------------------------------------------
	class ScriptVM
	{
	public:
		ScriptVM();
		~ScriptVM();

		//---- Interface ----------------------
		void execute		(const char* _code);					// Run some code in the virtual machine
		
		void setVar			(const char* _name, const Variant& _v);	// Set a variable in the virtual environment
		void getVar			(const char* _name, Variant& _v);		// Retrieve the variable's value from the virtual environment

	public:
		typedef	std::unordered_map<std::string, unsigned>	LookUpT;

	private:
		DataTable			mStoredData;
		LookUpT				mVarTable;

		ScriptVMBackend*	mBackend;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_SCRIPTVM_H_