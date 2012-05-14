////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// Generic lexical analyzer based on regular expression finite state machines

#ifndef _REV_CORE_INTERPRETER_LEXER_H_
#define _REV_CORE_INTERPRETER_LEXER_H_

#include "token.h"

#include <vector.h>

namespace rev
{
	class CRegExpFsm;

	struct CTokenRule
	{
		unsigned	token;
		const char*	rule;
		unsigned	minSize;
	};

	class CLexer
	{
	public:
		CLexer(const CTokenRule* _rules, unsigned _nRules);

		void tokenizeCode(rtl::vector<CToken>& _tokenList, const char * _code);

	private:
		unsigned	getToken(CToken& t, const char * _code);

	private:
		CRegExpFsm*	mRules;
		unsigned*	mTokens;
		unsigned	mNRules;
	};

}	// namespace rev

#endif // _REV_CORE_INTERPRETER_LEXER_H_