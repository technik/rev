//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 22nd, 2013
//-------------------------------------------------------------------------
// Virtual code execution structure

#include "executionTree.h"
#include "../../scriptVM.h"
#include "../../variant/variant.h"

#include <revCore/codeTools/assert/assert.h>

namespace rev { namespace script {

	//---------------------------------------------------------------------
	IntegerExpression::IntegerExpression(int _literal)
		:mValue(_literal)
	{
	}

	//---------------------------------------------------------------------
	void IntegerExpression::evaluate(Variant& _result) const
	{
		_result = mValue;
	}

	//---------------------------------------------------------------------
	RealExpression::RealExpression(float _literal)
		:mValue(_literal)
	{
	}

	//---------------------------------------------------------------------
	void RealExpression::evaluate(Variant& _result) const
	{
		_result = mValue;
	}

	//---------------------------------------------------------------------
	AssignStatement::AssignStatement(std::string* _identifier, Expression * _value)
		:mId(_identifier)
		,mExpression(_value)
	{
		// Check input data is valid
		revAssert(nullptr != _identifier);
		revAssert(_identifier->c_str()[0] != '\0');
		revAssert(nullptr != _value);
	}

	//---------------------------------------------------------------------
	AssignStatement::~AssignStatement()
	{
		if(nullptr != mId) {
			delete mId;
		}
		delete mExpression;
	}

	//---------------------------------------------------------------------
	void AssignStatement::run(ScriptVM* _vm) const
	{
		Variant value;
		mExpression->evaluate(value);
		_vm->setVar(mId->c_str(), value);
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