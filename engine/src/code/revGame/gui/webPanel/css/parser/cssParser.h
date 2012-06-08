////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 27th, 2012
////////////////////////////////////////////////////////////////////////////////
// css parsers

#ifndef _REV_GAME_GUI_WEBPANEL_CSS_CSSPARSER_H_
#define _REV_GAME_GUI_WEBPANEL_CSS_CSSPARSER_H_

#include <revCore/interpreter/parser.h>

namespace rev { namespace game
{
	class CCssDeclParser
	{
	public:
		static void		init();
		static CParser*	get();
		static void		end();

	private:
		static CParser*	sCssDeclParser;
	};

	class CCssFileParser
	{
	public:
		static void		init();
		static CParser*	get();
		static void		end();

	private:
		static CParser*	sCssFileParser;
	};
}	// namespace rev
}	// namespace game

#endif // _REV_GAME_GUI_WEBPANEL_CSS_CSSPARSER_H_
