//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 21st, 2013
//-------------------------------------------------------------------------
// Virtual code execution structure

#ifndef _REV_SCRIPT_VIRTUALMACHINE_EXECUTIONTREE_H_
#define _REV_SCRIPT_VIRTUALMACHINE_EXECUTIONTREE_H_

namespace rev { namespace script {

	class ScriptVM;
	
	class ExecutionTree
	{
	public:
		void run(ScriptVM* _vm) const;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_VIRTUALMACHINE_EXECUTIONTREE_H_