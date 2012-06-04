////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#ifndef _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_
#define _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_

#include <revCore/variant/variant.h>
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

		///\ Registers a function to the script machine
		//void	addFunction		(TScriptFunction _fn, const char * _name);
		void createContextLevel();
		void destroyContextLevel();
		unsigned getVar(const char * _name);
		void assign(const char * dst, const char * src);

	private:
		CScriptMachine();
		~CScriptMachine();

		unsigned getFirstEmptyVar();
		
	private:
		typedef rtl::pair<unsigned, CVariant>	TScriptVar;
		rtl::vector<TScriptVar>	mVarTable;
		typedef rtl::map<string, unsigned>	TCtxLevel;
		rtl::vector<TCtxLevel>	mContextStack;

		static CScriptMachine* sInstance;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_