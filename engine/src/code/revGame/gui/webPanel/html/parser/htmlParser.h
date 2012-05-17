////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// Html parser

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_HTMLPARSER_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_HTMLPARSER_H_

#include <revCore/interpreter/parser.h>

namespace rev { namespace game
{
	class CHtmlParser
	{
	public:
		static void		init();
		static CParser*	get();
		static void		end();

	private:
		static CParser*	sHtmlParser;
	};
}	// namespace rev
}	// namespace game

#endif // _REV_GAME_GUI_WEBPANEL_HTML_HTMLPARSER_H_
