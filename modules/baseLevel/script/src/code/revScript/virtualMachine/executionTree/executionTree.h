//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 21st, 2013
//-------------------------------------------------------------------------
// Virtual code execution structure

#ifndef _REV_SCRIPT_VIRTUALMACHINE_EXECUTIONTREE_H_
#define _REV_SCRIPT_VIRTUALMACHINE_EXECUTIONTREE_H_

#include <string>

namespace rev { namespace script {

	class ScriptVM;
	class Variant;

	//---------------------------------------------------------
	class Expression
	{
	public:
		virtual ~Expression() {}
		virtual void evaluate(Variant& _result) const = 0;
	};

	//---------------------------------------------------------
	class IntegerExpression : public Expression
	{
	public:
		IntegerExpression(int);
		void evaluate(Variant& _result) const;
	private:
		int mValue;
	};

	//---------------------------------------------------------
	class RealExpression : public Expression
	{
	public:
		RealExpression(float);
		void evaluate(Variant& _result) const;
	private:
		float mValue;
	};

	//---------------------------------------------------------
	class AssignStatement
	{
	public:
		AssignStatement(std::string* _identifier, Expression* _value);
		~AssignStatement();
		void run(ScriptVM* _vm) const;

	private:
		std::string*	mId;
		Expression*		mExpression;

	};
	
	//---------------------------------------------------------
	class ExecutionTree
	{
	public:
		ExecutionTree();
		~ExecutionTree();
		void run(ScriptVM* _vm) const;
		void addAssign(AssignStatement* _stmt);
	private:
		AssignStatement* mStmt;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_VIRTUALMACHINE_EXECUTIONTREE_H_