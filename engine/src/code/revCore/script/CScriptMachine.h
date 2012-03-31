////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#ifndef _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_
#define _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_

namespace rev
{
	class CVariant;

	class CScriptMachine
	{
	public:
		typedef void(*TScriptFunction)(const CVariant&,CVariant&);
		
	public:
		static void				init	();
		static void				end		();
		static CScriptMachine*	get		();

		///\ Registers a function to the script machine
		void	addFunction		(TScriptFunction _fn, const char * _name);
		///\ Evaluates an expression
		void	eval			(const char * _expression);
		///\ Evaluates an expression and stores the result in _dst variant
		void	eval			(const char * _expression, CVariant& _dst);

	private:
		CScriptMachine();
		~CScriptMachine();
	};
}	// rev

#endif // _REV_CORE_SCRIPT_CSCRIPTMACHINE_H_