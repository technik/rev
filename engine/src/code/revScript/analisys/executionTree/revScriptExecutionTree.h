////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on May 18th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Execution tree

#ifndef _REV_SCRIPT_ANALISYS_EXECUTIONTREE_REVSCRIPTEXECUTIONTREE_H_
#define _REV_SCRIPT_ANALISYS_EXECUTIONTREE_REVSCRIPTEXECUTIONTREE_H_

#include <vector.h>

#include <revCore/interpreter/parser.h>
#include <revCore/string.h>
#include <revCore/variant/variant.h>

namespace rev { namespace script
{
	/*

	class CStmtLstNode
	{
	public:
		CStmtLstNode(CParserNonLeaf*);
		void eval(CVariant& _v);
	private:
		rtl::vector<CStmtNode*>	mStmts;
	};*/

	//--------------------------------------------------------------------
	class CExpressionNode
	{
	public:
		static CExpressionNode * expressionNode(CParserNonLeaf*);
		virtual void eval(CVariant& _v) = 0;
	};

	//--------------------------------------------------------------------
	class CLiteralNode : public CExpressionNode
	{
	public:
		static CLiteralNode * literalNode(CParserNonLeaf*);
		virtual void eval(CVariant& _v) = 0;
	private:
	};

	//--------------------------------------------------------------------
	class CVectorNode : public CLiteralNode
	{
	public:
		CVectorNode(CParserNonLeaf*);
		void eval(CVariant& _v);
	private:
		rtl::vector<CExpressionNode*> mElements;
	};

	//--------------------------------------------------------------------
	class CStringNode : public CLiteralNode
	{
	public:
		CStringNode(CParserLeaf*);
		void eval(CVariant& _v);
	private:
		string mValue;
	};

	//--------------------------------------------------------------------
	class CFloatNode : public CLiteralNode
	{
	public:
		CFloatNode(CParserLeaf*);
		void eval (CVariant& _v);
	private:
		double mValue;
	};

	//--------------------------------------------------------------------
	class CStmtNode
	{
	public:
		class CStmtNode(CParserNonLeaf*);
		void eval(CVariant& _v);
	private:
		CExpressionNode * mExpression;
	};

	//--------------------------------------------------------------------
	class CFnDefinition
	{
	};

	//--------------------------------------------------------------------
	class CRSTree // Rev Script Tree
	{
	public:
		CRSTree(CParserNode*);
		void run();
		void eval(CVariant& _v);

	private:
		rtl::vector<CFnDefinition*>	mFnDefinitions;
		rtl::vector<CStmtNode*>	mStmts;
	};
}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_ANALISYS_EXECUTIONTREE_REVSCRIPTEXECUTIONTREE_H_