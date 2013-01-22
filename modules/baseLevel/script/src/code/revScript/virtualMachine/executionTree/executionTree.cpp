//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 22nd, 2013
//-------------------------------------------------------------------------
// Virtual code execution structure

#include "executionTree.h"
#include "../../scriptVM.h"
#include "../../variant/variant.h"

namespace rev { namespace script {

	//---------------------------------------------------------------------
	AssignStatement::AssignStatement(std::string* _identifier, int _literal)
		:mId(_identifier)
		,mLiteral(_literal)
	{ // Intentionally blank
	}

	//---------------------------------------------------------------------
	AssignStatement::~AssignStatement()
	{
		if(nullptr != mId) {
			delete mId;
		}
	}

	//---------------------------------------------------------------------
	void AssignStatement::run(ScriptVM* _vm) const
	{
		Variant v(mLiteral);
		_vm->setVar(mId->c_str(), mLiteral);
	}

	//---------------------------------------------------------------------
	ExecutionTree::ExecutionTree()
		:mStmt(nullptr)
	{
		// Intentionally blank
	}

	//---------------------------------------------------------------------
	ExecutionTree::~ExecutionTree()
	{
		if(nullptr != mStmt)
			delete mStmt;
	}

	//---------------------------------------------------------------------
	void ExecutionTree::run(ScriptVM* _vm) const
	{
		if(nullptr != mStmt)
			mStmt->run(_vm);
	}

	//---------------------------------------------------------------------
	void ExecutionTree::addAssign(AssignStatement* _stmt)
	{
		if(nullptr != mStmt)
			delete mStmt;
		mStmt = _stmt;
	}

}	// namespace script
}	// namespace rev