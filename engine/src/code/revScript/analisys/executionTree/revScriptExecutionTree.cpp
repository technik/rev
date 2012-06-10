////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// on May 18th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Execution tree

#include "revScriptExecutionTree.h"

#include <revCore/codeTools/log/log.h>
#include <revCore/variant/variant.h>
#include <revScript/scriptMachine.h>
#include <revScript/scriptSyntax.h>
#include <revScript/scriptToken.h>

namespace rev { namespace script
{
	//------------------------------------------------------------------------------------------------------------------
	CExpressionNode * CExpressionNode::expressionNode(CParserNonLeaf * _node)
	{
		CParserNode * child = _node->mChildren[0];
		if(child->mRule->from == eLiteral)
		{
			return CLiteralNode::literalNode(static_cast<CParserNonLeaf*>(child));
		}
		else if(child->mRule->from == eIdentifier)
		{
			return new CIdentifierExpression(static_cast<CParserLeaf*>(child));
		}
		else // Assignement
		{
			return 0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CIdentifierExpression::CIdentifierExpression(CParserLeaf * _node)
		:mIdentifier(_node->mToken.text)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CIdentifierExpression::eval(CScriptMachine * _sm)
	{
		return _sm->findData(mIdentifier.c_str());
	}

	//------------------------------------------------------------------------------------------------------------------
	CLiteralNode * CLiteralNode::literalNode(CParserNonLeaf * _node)
	{
		CParserNode * child = _node->mChildren[0];
		if(child->mRule) // Not a terminal
		{
			if(child->mRule->from == eInlineVector)
			{
				return new CVectorNode(static_cast<CParserNonLeaf*>(child));
			}
			else
			{
				revLogN("Error: literals can only be strings, floats or vectors");
				codeTools::SLog::get()->flush();
				return 0;
			}
		}
		else // Terminal literal
		{
			CParserLeaf * leaf = static_cast<CParserLeaf*>(child);
			if(leaf->mToken.type == eString)
			{
				return new CStringNode(static_cast<CParserLeaf*>(child));
			}
			else if(leaf->mToken.type == eFloat)
			{
				return new CFloatNode(static_cast<CParserLeaf*>(child));
			}
			else
			{
				revLogN("Error: literals can only be strings, floats or vectors");
				codeTools::SLog::get()->flush();
				return 0;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CVectorNode::CVectorNode(CParserNonLeaf* _node)
	{
		CParserNonLeaf * expr = static_cast<CParserNonLeaf*>(_node->mChildren[1]);
		while(expr->mRule->to.size() > 1) // expLst : expr , exprL
		{
			mElements.push_back(CExpressionNode::expressionNode(static_cast<CParserNonLeaf*>(expr->mChildren[0])));
			expr = static_cast<CParserNonLeaf*>(expr->mChildren[2]);
		}
		if(!expr->mRule->to.empty()) // exprLst : expr
		{
			mElements.push_back(CExpressionNode::expressionNode(static_cast<CParserNonLeaf*>(expr->mChildren[0])));
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CVectorNode::eval(CScriptMachine * _sm)
	{
		rtl::vector<unsigned>	elementIndices;
		for(rtl::vector<CExpressionNode*>::iterator i = mElements.begin(); i != mElements.end(); ++i)
		{
			elementIndices.push_back((*i)->eval(_sm));
		}
		return _sm->addData(elementIndices);
	}

	//------------------------------------------------------------------------------------------------------------------
	CStringNode::CStringNode(CParserLeaf* _node)
		:mValue(&_node->mToken.text[1])
	{
		// Remove quotes from the string
		mValue.pop_back();
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CStringNode::eval(CScriptMachine * _sm)
	{
		CVariant value = CVariant(mValue);
		return _sm->addData(value);
	}

	//------------------------------------------------------------------------------------------------------------------
	CFloatNode::CFloatNode(CParserLeaf* _node)
	{
		mValue = doubleFromString(_node->mToken.text);
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CFloatNode::eval(CScriptMachine * _sm)
	{
		CVariant value = CVariant(mValue);
		return _sm->addData(value);
	}

	//------------------------------------------------------------------------------------------------------------------
	CStmtNode * CStmtNode::stmtNode(CParserNonLeaf * _node)
	{
		CParserLeaf * _tk = static_cast<CParserLeaf*>(_node->mChildren[1]);
		if(_tk->mToken.type == eAssignOperator) // Assignement
		{
			return new CAssignStmt(_node);
		}
		else
			return new CFnCallStmt(_node);
	}

	//------------------------------------------------------------------------------------------------------------------
	CAssignStmt::CAssignStmt(CParserNonLeaf* _node)
	{
		CParserLeaf * id = static_cast<CParserLeaf*>(_node->mChildren[0]);
		CParserNonLeaf * expr = static_cast<CParserNonLeaf*>(_node->mChildren[2]);
		mIdentifier = string(id->mToken.text);
		mExpression = CExpressionNode::expressionNode(expr);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CAssignStmt::run(CScriptMachine * _sm)
	{
		_sm->setVar(mIdentifier.c_str(), mExpression->eval(_sm));
	}

	//------------------------------------------------------------------------------------------------------------------
	CFnCallStmt::CFnCallStmt(CParserNonLeaf * _node)
	{
		CParserLeaf * id = static_cast<CParserLeaf*>(_node->mChildren[0]);
		mIdentifier = string(id->mToken.text);
		CParserNonLeaf * expr = static_cast<CParserNonLeaf*>(_node->mChildren[2]);
		while(expr->mRule->to.size() > 1) // expLst : expr , exprL
		{
			mArgs.push_back(CExpressionNode::expressionNode(static_cast<CParserNonLeaf*>(expr->mChildren[0])));
			expr = static_cast<CParserNonLeaf*>(expr->mChildren[2]);
		}
		if(!expr->mRule->to.empty()) // exprLst : expr
		{
			mArgs.push_back(CExpressionNode::expressionNode(static_cast<CParserNonLeaf*>(expr->mChildren[0])));
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CFnCallStmt::run(CScriptMachine * _sm)
	{
		rtl::vector<unsigned>	arguments;
		for(rtl::vector<CExpressionNode*>::iterator i = mArgs.begin(); i != mArgs.end(); ++i)
		{
			arguments.push_back((*i)->eval(_sm));
		}
		_sm->callFunction(mIdentifier.c_str(), arguments);
		// TODO: maybe we should free the return value from current context
	}

	//------------------------------------------------------------------------------------------------------------------
	CRSTree::CRSTree(CParserNode * _node)
	{
		CParserNonLeaf * root= static_cast<CParserNonLeaf*>(_node);
		CParserNonLeaf * child = static_cast<CParserNonLeaf*>(root->mChildren[0]);
		while(child->mRule->to.size() == 2)
		{
			mStmts.push_back(CStmtNode::stmtNode(static_cast<CParserNonLeaf*>(child->mChildren[0])));
			child = static_cast<CParserNonLeaf*>(child->mChildren[1]);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CRSTree::run()
	{
		for(rtl::vector<CStmtNode*>::iterator i = mStmts.begin(); i != mStmts.end(); ++i)
		{
			(*i)->run(CScriptMachine::get());
		}
	}
}	// namespace script
}	// namespace rev