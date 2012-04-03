////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Sintax definition
#ifndef _REV_CORE_SCRIPT_SYNTAX_H_
#define _REV_CORE_SCRIPT_SYNTAX_H_

#include <vector.h>

#include "variant.h"

namespace rev { namespace script
{
	typedef void(*TScriptFunction)(const CVariant&,CVariant&);

	class IExpression
	{
	public:
		virtual ~IExpression() {}
		virtual void evaluate(CVariant& _result) const = 0;
	};

	class CConstantExpression : public IExpression
	{
	private:
		void evaluate(CVariant& _result) const
		{
			_result = mConstant;
		}
	public:
		CVariant mConstant;
	};

	class CListExpression : public IExpression
	{
	public:
		void evaluate(CVariant& _result) const
		{
			_result.setNill();
			CVariant _tmp;
			for(unsigned i = 0; i < mList.size(); ++i)
			{
				mList[i]->evaluate(_tmp);
				_result.append(_tmp);
			}
		}
		void addElement(IExpression * _element)
		{
			mList.push_back(_element);
		}
	private:
		rtl::vector<IExpression*>	mList;
	};

	class CVariable : public IExpression
	{
	private:
		void evaluate(CVariant& _result) const
		{
			_result = mValue;
		}
		CVariant mValue;
	};

	class CFunctionCall : public IExpression
	{
	private:
		void evaluate(CVariant& _result) const;

		rtl::vector<IExpression*>	mArguments;
		TScriptFunction				mFunction;
	};

	class CAssignment : public IExpression
	{
	private:
		void evaluate(CVariant& _result) const;
		IExpression * mDst, * mSrc;
	};

	class IStatement
	{
	public:
		virtual ~IStatement() {}
		virtual void execute(CVariant& _result) const = 0;
	};

	class CSingleExpressionStatement : public IStatement
	{
	private:
		void execute(CVariant& _result) const
		{
			mExpression->evaluate(_result);
		}
	public:
		IExpression * mExpression;
	};

	class CIfStatement : public IStatement
	{
	private:
		void execute(CVariant& _result) const
		{
			CVariant cond;
			mCondition->evaluate(cond);
			if(cond.asBool())
			{
				mStatement->execute(_result);
			}
		}
	public:
		IExpression * mCondition;
		IStatement * mStatement;
	};

	class CIfElseStatement : public IStatement
	{
	private:
		void execute(CVariant& _result) const
		{
			mCondition->evaluate(_result);
			if(_result.asBool())
				mPositiveStatement->execute(_result);
			else
				mNegativeStatement->execute(_result);
		}

	public:
		IExpression * mCondition;
		IStatement * mPositiveStatement;
		IStatement * mNegativeStatement;
	};

	class CStatementList
	{
	public:
		void execute(CVariant& _result) const
		{
			for(unsigned i = 0; i < mStatements.size(); ++i)
				mStatements[i]->execute(_result);
		}
		void addStatement(IStatement * statement)
		{
			mStatements.push_back(statement);
		}
	private:
		rtl::vector<IStatement*>	mStatements;
	};

	class CCodeBlockStatement : public IStatement
	{
	private:
		void execute(CVariant& _result) const
		{
			mStatementList.execute(_result);
		}
	public:
		CStatementList	mStatementList;
	};
}	// namespace script
}	// namespace rev

#endif // _REV_CORE_SCRIPT_SYNTAX_H_