////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#include "script.h"

#include "analisys/lexer/lexicalAnalizer.h"
#include "analisys/parser/syntaxParser.h"

namespace rev { namespace script
{
	//------------------------------------------------------------------------------------------------------------------
	CScript::CScript(const char * _code)
	{
		// Parse code and generate a list of tokens
		rtl::vector<CScriptToken>	tokens;
		if(SLexicalAnalizer::parseCodeIntoTokens(_code, tokens) != -1)
		{
			// Transform the list of tokens into
			SSyntaxParser::parseTokenListIntoExecutionTree(mCode, tokens);
		}

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
