////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lexical analizer of scripts
#ifndef _REV_SCRIPT_ANALISYS_LEXER_SLEXICALANALIZER_H_
#define _REV_SCRIPT_ANALISYS_LEXER_SLEXICALANALIZER_H_

#include <vector.h>

#include <revCore/interpreter/lexer.h>
#include <revScript/scriptToken.h>

namespace rev { namespace script
{
	class CRevScriptLexer
	{
	public:
		static void init();
		static CLexer * get();
		static void end();

	private:
		static CLexer * sLexer;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_ANALISYS_LEXER_SLEXICALANALIZER_H_
