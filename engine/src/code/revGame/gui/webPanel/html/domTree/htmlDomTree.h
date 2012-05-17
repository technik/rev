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

namespace rev { struct CParserNode; }
namespace rev {namespace video { class CFont; } }

namespace rev { namespace game
{
	class CHtmlRenderContext;

	class CHtmlDomNode
	{
	public:
		static CHtmlDomNode * createNode(CParserNode * _tree);
		virtual void render(CHtmlRenderContext& _context, unsigned _x, unsigned _y) const;
		void addChild(CHtmlDomNode* _child);
	protected:
	private:
		static CHtmlDomNode * htmlNode(CParserNonLeaf * _node);
		
		rtl::vector<CHtmlDomNode*> mChildren;
	};

	class CHtmlElementNode : public CHtmlDomNode
	{
	public:
		CHtmlElementNode(const char * _text): mText(_text) {}
		CHtmlElementNode() {}
		void render(CHtmlRenderContext& _context, unsigned _x, unsigned _y) const;
	private:
		string mText;
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_DOMTREE_HTMLDOMTREE_H_