////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#ifndef _REV_CORE_SCRIPT_CSCRIPT_H_
#define _REV_CORE_SCRIPT_CSCRIPT_H_

// Forward declarations
namespace rev { class CVariant; }

namespace rev { namespace script
{
	class CRSTree;

	class CScript
	{
	public:
		CScript(const char * _filename);
		~CScript() {}

		void run(CVariant& _res);

	private:
		CRSTree * mExecTree;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_CORE_SCRIPT_CSCRIPT_H_
