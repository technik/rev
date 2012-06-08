////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 26th, 2012
////////////////////////////////////////////////////////////////////////////////
// css lexer

#ifndef _REV_GAME_GUI_WEBPANEL_CSS_LEXER_CSSLEXER_H_
#define _REV_GAME_GUI_WEBPANEL_CSS_LEXER_CSSLEXER_H_

#include <revCore/interpreter/lexer.h>

namespace rev { namespace game
{
	class CCssFileLexer
	{
	public:
		static void init();
		static CLexer * get();
		static void end();
	private:
		static CLexer * sCssFileLexer;
	};

	class CCssDeclarationLexer
	{
	public:
		static void init();
		static CLexer * get();
		static void end();
	private:
		static CLexer * sCssDeclarationLexer;
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_CSS_LEXER_CSSLEXER_H_
