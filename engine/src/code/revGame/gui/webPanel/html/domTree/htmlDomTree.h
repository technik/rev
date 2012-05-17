////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 11th, 2012
////////////////////////////////////////////////////////////////////////////////
// html Dom Tree

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_DOMTREE_HTMLDOMTREE_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_DOMTREE_HTMLDOMTREE_H_

#include <revGame/gui/webPanel/html/htmlTokens.h>
#include <vector.h>

namespace rev { class CParserNode; }

namespace rev { namespace game
{
	class CHtmlDomNode
	{
	public:
		static CHtmlDomNode * createNode(CParserNode * _tree);
		virtual void render(unsigned char * _dstImg, unsigned _w, unsigned _h, unsigned _x, unsigned _y) const;
	private:
		rtl::vector<CHtmlDomNode*> mChildren;
	}
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_DOMTREE_HTMLDOMTREE_H_