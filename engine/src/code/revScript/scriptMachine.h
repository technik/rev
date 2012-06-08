////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#ifndef _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_
#define _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_

#include <revCore/variant/variant.h>
#include <dictionary.h>
#include <vector.h>
#include <rtl/map.h>

// Forward declarations
namespace rev { class CVariant; }

namespace rev { namespace script
{
	class CScriptMachine
	{
	public:
		static void				init	();
		static void				end		();
		static CScriptMachine*	get		();

		// Returns true if the function writes something to _ret
		typedef bool(*TScriptFunction)(CVariant& _args, CVariant& _ret);

		///\ Registers a function to the script machine
		void	addFunction		(TScriptFunction _fn, const char * _name);
		void	callFunction	(const char * _name, CVariant& _args, CVariant& _ret);
//		void createContextLevel();
//		void destroyContextLevel();
		unsigned getVar(const char * _name);
		void setVar(const char * _name, unsigned _idx);
		unsigned addvar(CVariant& _v);
		void getValue(unsigned _idx, CVariant& _v);
		void getValue(const char * _varName, CVariant& _dst);

	private:
		CScriptMachine();
		~CScriptMachine();

		unsigned getFirstEmptyVar();
		
	private:
		typedef rtl::pair<unsigned, CVariant>	TScriptVar;
		typedef rtl::map<string, unsigned>		TCtxLevel;

		rtl::dictionary<TScriptFunction, 64>	mCoreFunctions; // Functions registered from code
		rtl::vector<TScriptVar>					mVarTable;
		rtl::vector<TCtxLevel>					mContextStack;

		static CScriptMachine* sInstance;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_