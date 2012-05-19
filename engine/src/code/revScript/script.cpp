////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#include "script.h"

#include "analisys/executionTree/revScriptExecutionTree.h"
#include "analisys/lexer/lexicalAnalizer.h"
#include "analisys/parser/syntaxParser.h"

namespace rev { namespace script
{
	//------------------------------------------------------------------------------------------------------------------
	CScript::CScript(const char * _code)
	{
		// Parse code and generate a list of tokens
		rtl::vector<CToken>	tokens;
		CRevScriptLexer::get()->tokenizeCode(tokens, _code);
		CRevScriptParser::get()->stripTokens(tokens, eSpace);
		CRevScriptParser::get()->stripTokens(tokens, eCommentToken);
		CParserNode * nodeTree = CRevScriptParser::get()->generateParseTree(tokens);
		mExecTree = new CRSTree(nodeTree);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScript::run(CVariant& _res)
	{
		mExecTree->eval(_res);
	}
}	// namespace script
}	// namespace rev
