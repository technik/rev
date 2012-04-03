////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#include "script.h"

#include "lexer/lexicalAnalizer.h"
#include "parser/syntaxParser.h"

namespace rev { namespace script
{
	//------------------------------------------------------------------------------------------------------------------
	CScript::CScript(const char * _code)
	{
		// Parse code and generate a list of tokens
		rtl::vector<CScriptToken>	tokens;
		SLexicalAnalizer::parseCodeIntoTokens(_code, tokens);
		// Transform the list of tokens into
		SSyntaxParser::parseTokenListIntoExecutionTree(mCode, tokens);

		// House keeping
		for(unsigned i = 0; i < tokens.size(); ++i)
		{
			delete[] tokens[i].mContent;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScript::run(CVariant& _res)
	{
		mCode.execute(_res);
	}
}	// namespace script
}	// namespace rev
