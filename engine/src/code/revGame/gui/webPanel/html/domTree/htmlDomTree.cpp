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
#include <revGame/gui/webPanel/html/htmlTokens.h>
#include <revGame/gui/webPanel/html/htmlExpressions.h>
#include <revGame/gui/webPanel/html/renderContext/htmlRenderContext.h>
#include <revVideo/font/font.h>
#include <revVideo/texture/texture.h>

using namespace rev::video;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CHtmlDomNode * CHtmlDomNode::createNode(CParserNode * _node)
	{
		const CGrammarRule * rule = _node->mRule;
		rule;
		if(rule->from == eDocument)
		{
			CParserNonLeaf * root = static_cast<CParserNonLeaf*>(_node);
			CParserNonLeaf * body = static_cast<CParserNonLeaf*>(root->mChildren[1]);
			return createNode(body);
		}
		else if(rule->from == eBody)
		{
			CParserNonLeaf * body = static_cast<CParserNonLeaf*>(_node);
			return htmlNode(static_cast<CParserNonLeaf*>(body->mChildren[1]));
		}
		// if(rule->from == eH
		return new CHtmlDomNode();
	}

	//------------------------------------------------------------------------------------------------------------------
	CHtmlDomNode* CHtmlDomNode::htmlNode(CParserNonLeaf * _node)
	{
		if(_node->mChildren.size() == 2) // W H
		{
			CHtmlElementNode * node = new CHtmlElementNode(static_cast<CParserLeaf*>(_node->mChildren[0])->mToken.text);
			node->addChild(htmlNode(static_cast<CParserNonLeaf*>(_node->mChildren[1])));
			return node;
		}
		else
		{
			return new CHtmlElementNode();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlDomNode::addChild(CHtmlDomNode * _child)
	{
		mChildren.push_back(_child);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlDomNode::render(CHtmlRenderContext& _context, unsigned _x, unsigned _y) const
	{
		for(rtl::vector<CHtmlDomNode*>::const_iterator i = mChildren.begin(); i != mChildren.end(); ++i)
		{
			(*i)->render(_context, _x, _y);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlElementNode::render(CHtmlRenderContext& _context, unsigned _x, unsigned _y) const
	{
		if(mText.empty())
		{
			_context.renderText(_x, _y);
		}
		else
		{
			_context.addText(mText.c_str());
		}
		CHtmlDomNode::render(_context, _x, _y);
	}

}	// namespace game
}	// namespace rev