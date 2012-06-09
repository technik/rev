////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#ifndef _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_
#define _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_

#include <revCore/variant/variant.h>
#include <revScript/scriptMachine/varTable.h>
#include <dictionary.h>
#include <vector.h>

// Forward declarations
namespace rev { class CVariant; }

namespace rev { namespace script
{
	class CScriptMachine
	{
	public:
		// Compliant functions must return true if the function writes something to _ret
		typedef bool(*TScriptFunction)(CVariant& _args, CVariant& _ret);

	public:
		// Singleton management
		static void				init	();
		static void				end		();
		static CScriptMachine*	get		();

		// ----- Dealing with functions -----
		///\ Registers a function to the script machine
		void		addFunction		(TScriptFunction _fn, const char * _name);
		///\return: index pointing to the return value of the function (0 if the function returns nothing)
		unsigned	callFunction	(const char * _name,	// Function name
						const rtl::vector<unsigned>& _argIndices);// List of indices pointing to the variables that store argument values

		// ----- Dealing with data -----
		void getVar			(const char * _name, CVariant& _value);
		void setVar			(const char * _name, const CVariant& _value);
		void setVar			(const char * _name, unsigned _idx);
		void setVar			(const char * _name, const rtl::vector<unsigned>& _indices);
		unsigned addData	(const CVariant& _data);
		unsigned addData	(const rtl::vector<unsigned>& _indices);
		unsigned findData	(const char * _name);

	private:
		CScriptMachine();
		~CScriptMachine();
		
	private:
		// Custom types
		typedef rtl::dictionary<unsigned, 64>			contextT;
		typedef rtl::dictionary<TScriptFunction, 64>	functionIndexT;

		// Internal data
		functionIndexT	mCoreFunctions; // Functions registered from code
		contextT		mContext;		// Execution context, keeps variables indexed by name
		CVarTable		mData;			// Variable table containing the actual raw data

		// Singleton instance
		static CScriptMachine* sInstance;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_