////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script machine

#include "script.h"
#include "scriptMachine.h"

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
		CRevScriptParser::get()->stripTokens(tokens, eLineComment);
		CRevScriptParser::get()->stripTokens(tokens, eBlockComment);
		CParserNode * nodeTree = CRevScriptParser::get()->generateParseTree(tokens);
		mExecTree = new CRSTree(nodeTree);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScript::run()
	{
		mExecTree->run();
	}

	//------------------------------------------------------------------------------------------------------------------
	void CScript::getVar(const char * _name, CVariant& _v)
	{
		CScriptMachine::get()->getValue(_name, _v);
	}
}	// namespace script
}	// namespace rev
