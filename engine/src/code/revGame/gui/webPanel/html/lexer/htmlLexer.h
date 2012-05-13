////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 11th, 2012
////////////////////////////////////////////////////////////////////////////////
// html lexer

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_LEXER_HTMLLEXER_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_LEXER_HTMLLEXER_H_

#include <vector.h>
#include <revGame/gui/webPanel/html/htmlTokens.h>

namespace rev { namespace game
{
	class CHtmlLexer
	{
	public:
		static void parseHtmlIntoTokens(const char * _html, rtl::vector<CHtmlToken>& _tokens);

	private:
		static bool	isANumber(char character);
		static bool isALetter(char character);
		static bool isAlphanumeric(char character);
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_LEXER_HTMLLEXER_H_
