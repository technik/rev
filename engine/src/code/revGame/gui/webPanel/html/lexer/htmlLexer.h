////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 11th, 2012
////////////////////////////////////////////////////////////////////////////////
// html lexer

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_LEXER_HTMLLEXER_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_LEXER_HTMLLEXER_H_

#include <revCore/interpreter/lexer.h>

namespace rev { namespace game
{
	class CHtmlLexer
	{
	public:
		static void init();
		static CLexer * get();
		static void end();
	private:
		static CLexer * sHtmlLexer;
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_LEXER_HTMLLEXER_H_
