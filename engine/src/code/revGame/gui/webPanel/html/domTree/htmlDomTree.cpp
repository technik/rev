////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 17th, 2012
////////////////////////////////////////////////////////////////////////////////
// html Dom Tree

#include "htmlDomTree.h"

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/interpreter/parser.h>
#include <revCore/interpreter/token.h>
#include <revGame/gui/webPanel/css/cssDeclaration.h>
#include <revGame/gui/webPanel/html/htmlTokens.h>
#include <revGame/gui/webPanel/html/htmlExpressions.h>
#include <revGame/gui/webPanel/html/renderContext/htmlRenderContext.h>
#include <revVideo/font/font.h>
#include <revVideo/texture/texture.h>

using namespace rev::video;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CHtmlElementNode::CHtmlElementNode(CParserNode * _node)
	{
		CParserNonLeaf * element = static_cast<CParserNonLeaf*>(_node);
		// Has style argument?
		CParserNonLeaf * argList = static_cast<CParserNonLeaf*>(element->mChildren[2]);
		while(argList->mChildren.size() > 0)
		{
			// Argument
			CParserNonLeaf * argument = static_cast<CParserNonLeaf*>(argList->mChildren[0]);
			// Value
			//CParserLeaf * value = static_cast<CParserLeaf*>(argument->mChildren[2]);
			// Attribute
			CParserNonLeaf * attribute = static_cast<CParserNonLeaf*>(argument->mChildren[0]);
			if(attribute->mRule->to[0].value == eStyleAttrib)
			{
				mStyle = new CCssDeclaration("color:red;");
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CHtmlSpanElement::CHtmlSpanElement(CParserNode *_node)
		:CHtmlElementNode(_node)
	{
		// Process htmlCode as children
		CParserNonLeaf * element = static_cast<CParserNonLeaf*>(_node);
		CParserNonLeaf * code = static_cast<CParserNonLeaf*>(element->mChildren[4]);
		while(code->mRule->to.size() == 2) // Non-empty code
		{
			// word or element
			CParserNode * childNode = code->mChildren[0];
			if(childNode->mRule->from == eHtmlElement)
			{
				mChildren.push_back(new CHtmlSpanElement(childNode));
			}
			else // word
			{
				mChildren.push_back(new CHtmlWordNode(childNode));
			}
			// Next child
			code = static_cast<CParserNonLeaf*>(code->mChildren[1]);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlSpanElement::render(CHtmlRenderContext& _context)
	{
		if(0 != mStyle)
			_context.addStyle(*mStyle);
		for(unsigned i = 0; i < mChildren.size(); ++i)
			mChildren[i]->render(_context);
		if(0 != mStyle)
		{
			_context.renderText();
			_context.removeTopStyle();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CHtmlWordNode::CHtmlWordNode(CParserNode * _node)
		:CHtmlElementNode(_node)
	{
		CParserLeaf * word = static_cast<CParserLeaf*>(_node);
		mText = word->mToken.text;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlWordNode::render(CHtmlRenderContext& _context)
	{
		_context.addText(mText.c_str());
	}

	//------------------------------------------------------------------------------------------------------------------
	CHtmlBodyNode::CHtmlBodyNode(CParserNode * _node)
	{
		CParserNonLeaf * body = static_cast<CParserNonLeaf*>(_node);
		// Process style
		CParserNonLeaf * argList = static_cast<CParserNonLeaf*>(body->mChildren[2]);
		while(argList->mChildren.size() > 0)
		{
			// Argument
			CParserNonLeaf * argument = static_cast<CParserNonLeaf*>(argList->mChildren[0]);
			// Value
			//CParserLeaf * value = static_cast<CParserLeaf*>(argument->mChildren[2]);
			// Attribute
			CParserNonLeaf * attribute = static_cast<CParserNonLeaf*>(argument->mChildren[0]);
			if(attribute->mRule->to[0].value == eStyleAttrib)
			{
				mStyle = new CCssDeclaration("color:red;");
			}
		}
		// Process children
		CParserNonLeaf * code = static_cast<CParserNonLeaf*>(body->mChildren[4]);
		while(code->mRule->to.size() == 2) // Non-empty code
		{
			// word or element
			CParserNode * childNode = code->mChildren[0];
			if(childNode->mRule->from == eHtmlElement)
			{
				mChildren.push_back(new CHtmlSpanElement(childNode));
			}
			else // word
			{
				mChildren.push_back(new CHtmlWordNode(childNode));
			}
			// Next child
			code = static_cast<CParserNonLeaf*>(code->mChildren[1]);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlBodyNode::render(CHtmlRenderContext& _context)
	{
		if(0 != mStyle)
			_context.addStyle(*mStyle);
		for(unsigned i = 0; i < mChildren.size(); ++i)
			mChildren[i]->render(_context);
		_context.renderText();
		if(0 != mStyle)
			_context.removeTopStyle();
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlDocument::render(CHtmlRenderContext& _context)
	{
		_context;
	};

}	// namespace game
}	// namespace rev
