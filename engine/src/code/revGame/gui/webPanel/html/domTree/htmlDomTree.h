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

namespace rev { namespace game
{
	///\ A Simplified dom tree
	/*class CHtmlDomTree
	{
	public:
		void loadFromTokenList(const rtl::vector<CHtmlToken>& _tokenList);
		void renderToImage(unsigned char * _dstImg, unsigned _dstWidth, unsigned _dstHeight);

	private:
		void consumeDoctype(rtl::vector<CHtmlToken>::const_iterator& _token);
		static void htmlAssert(bool _condition, const char * _errorMessage, unsigned _line, unsigned _pos);
	private:
		class CElement
		{
		public:
			//virtual void render(unsigned char * _dst, unsigned _x, unsigned _y, unsigned _w, unsigned _h) = 0;
			rtl::vector<CElement*> children;
		};

		CElement * mBody;
	};*/
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_DOMTREE_HTMLDOMTREE_H_