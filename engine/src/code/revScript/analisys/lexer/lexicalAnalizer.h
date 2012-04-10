////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lexical analizer of scripts
#ifndef _REV_CORE_SCRIPT_LEXER_SLEXICALANALIZER_H_
#define _REV_CORE_SCRIPT_LEXER_SLEXICALANALIZER_H_

#include <vector.h>

#include <revCore/script/scriptToken.h>

namespace rev { namespace script
{
	class SLexicalAnalizer
	{
	public:
		///\ parseCodeIntoTokens:
		///\ return -1 if no errors, or the position of the first character of an unrecognized token in case any.
		static int parseCodeIntoTokens(const char * code, rtl::vector<CScriptToken>& _tokenList);

	private:
		static bool	isANumber(char character);
		static bool isALetter(char character);
		static bool isAlphanumeric(char character);

		static int	processNumberToken		(const char * _code, rtl::vector<CScriptToken>& _tokenList);
		static int	processIdentifierToken	(const char * _code, rtl::vector<CScriptToken>& _tokenList);
		static int	skipComment				(const char * _code);

		static void logErrorMessageAndCode	(const char * _code, unsigned cursor);

		static CScriptToken	createToken(CScriptToken::ETokenType, const char * _code, unsigned length);

	private:
		static const char * sOperators;
		static const char * sSeparators;
	};

}	// namespace script
}	// namespace rev

#endif // _REV_CORE_SCRIPT_LEXER_SLEXICALANALIZER_H_
