////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#ifndef _REV_CORE_SCRIPT_CSCRIPT_H_
#define _REV_CORE_SCRIPT_CSCRIPT_H_

namespace rev
{
	class CVariant;

	class CScript
	{
	public:
		CScript(const char * _filename);
		~CScript();

		void run(const CVariant& _res);
	};
}	// rev

#endif // _REV_CORE_SCRIPT_CSCRIPT_H_
