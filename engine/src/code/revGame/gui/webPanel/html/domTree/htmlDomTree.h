////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 11th, 2012
////////////////////////////////////////////////////////////////////////////////
// html Dom Tree

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_DOMTREE_HTMLDOMTREE_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_DOMTREE_HTMLDOMTREE_H_

#include <revCore/interpreter/token.h>
#include <revCore/interpreter/parser.h>
#include <revCore/string.h>
#include <revGame/gui/webPanel/html/htmlTokens.h>
#include <vector.h>

namespace rev { class CParserNode; }
namespace rev {namespace video { class CFont; } }

namespace rev { namespace game
{
	class CHtmlRenderContext;
	class CCssDeclaration;

	//---------------------------------------------------------
	class CHtmlElementNode
	{
	public:
		CHtmlElementNode(CParserNode * _node);
		virtual void render(CHtmlRenderContext& _rc) = 0;

	protected:
		CCssDeclaration * mStyle;
	};

	//---------------------------------------------------------
	class CHtmlSpanElement : public CHtmlElementNode
	{
	public:
		CHtmlSpanElement(CParserNode * _node);
		void render(CHtmlRenderContext& _rc);
	private:
		rtl::vector<CHtmlElementNode*>	mChildren;
	};

	//---------------------------------------------------------
	class CHtmlWordNode : public CHtmlElementNode
	{
	public:
		CHtmlWordNode(CParserNode * _node);
		void render(CHtmlRenderContext& _rc);
	private:
		string	mText;
	};

	//---------------------------------------------------------
	class CHtmlBodyNode
	{
	public:
		CHtmlBodyNode(CParserNode * _bodyTree);
		void render(CHtmlRenderContext& _rc);

	private:
		rtl::vector<CHtmlElementNode*>	mChildren;
		CCssDeclaration * mStyle;
	};

	//---------------------------------------------------------
	class CHtmlDocument
	{
	public:
		CHtmlDocument(CParserNode * _htmlTree);
		void render(CHtmlRenderContext& _rc);
	private:
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_DOMTREE_HTMLDOMTREE_H_