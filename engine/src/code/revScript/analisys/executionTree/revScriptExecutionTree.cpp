////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
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
		else
		{
			revLog("Error: revScipt only supports error literal expressions", eError);
			codeTools::SLog::get()->flush();
			return 0;
		}
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
	void CVectorNode::eval(CScriptMachine * _sm, CVariant& _v)
	{
		for(rtl::vector<CExpressionNode*>::iterator i = mElements.begin(); i != mElements.end(); ++i)
		{
			CVariant e;
			(*i)->eval(_sm, e);
			_v.append(e);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CStringNode::CStringNode(CParserLeaf* _node)
		:mValue(&_node->mToken.text[1])
	{
		// Remove quotes from the string
		mValue.pop_back();
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStringNode::eval(CScriptMachine * _sm, CVariant& _v)
	{
		_sm;
		_v = mValue;
	}

	//------------------------------------------------------------------------------------------------------------------
	CFloatNode::CFloatNode(CParserLeaf* _node)
	{
		mValue = doubleFromString(_node->mToken.text);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CFloatNode::eval(CScriptMachine * _sm, CVariant& _v)
	{
		_sm;
		_v = mValue;
	}

	//------------------------------------------------------------------------------------------------------------------
	CStmtNode::CStmtNode(CParserNonLeaf* _node)
	{
		mExpression = CExpressionNode::expressionNode(static_cast<CParserNonLeaf*>(_node->mChildren[0]));
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStmtNode::eval(CScriptMachine * _sm, CVariant& _v)
	{
		mExpression->eval(_sm, _v);
	}

	//------------------------------------------------------------------------------------------------------------------
	/*CStmtLstNode::CStmtLstNode(CParserNonLeaf* _node)
	{
		CParserNonLeaf * child = _node;
		while(child->mRule->to.size() == 2)
		{
			mStmts.push_back(new CStmtNode(static_cast<CParserNonLeaf*>(child->mChildren[0])));
			child = static_cast<CParserNonLeaf*>(child->mChildren[1]);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CStmtLstNode::eval(CVariant& _v)
	{
		for(rtl::vector<CStmtNode*>::iterator i = mStmts.begin(); i != mStmts.end(); ++i)
		{
			(*i)->eval(_v);
		}
	}*/

	//------------------------------------------------------------------------------------------------------------------
	CRSTree::CRSTree(CParserNode * _node)
	{
		CParserNonLeaf * root= static_cast<CParserNonLeaf*>(_node);
		CParserNonLeaf * child = static_cast<CParserNonLeaf*>(root->mChildren[0]);
		while(child->mRule->to.size() == 2)
		{
			mStmts.push_back(new CStmtNode(static_cast<CParserNonLeaf*>(child->mChildren[0])));
			child = static_cast<CParserNonLeaf*>(child->mChildren[1]);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CRSTree::eval(CVariant& _v)
	{
		for(rtl::vector<CStmtNode*>::iterator i = mStmts.begin(); i != mStmts.end(); ++i)
		{
			(*i)->eval(CScriptMachine::get(), _v);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CRSTree::run()
	{
		CVariant v;
		eval(v);
	}
}	// namespace script
}	// namespace rev