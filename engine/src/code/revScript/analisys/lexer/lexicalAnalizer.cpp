////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 1st, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Lexical analizer of scripts

#include "lexicalAnalizer.h"

#include <revCore/interpreter/token.h>
#include <revScript/scriptToken.h>

namespace rev { namespace script
{
	const CTokenRule revScriptTokens[] = 
	{
		{eSpace,			" |\t|\n|\r|\f|\v+"},
		{eLineComment,		"//[^\n\r\f\v]*\n|\r|\f|\v"},
		{eBlockComment,		"#[^#]*#"},
		{eSemicolon,		";"},
		{eFunctionKW,		"function"},
		{eOpenParenthesis,	"("},
		{eCloseParenthesis,	")"},
		{eComma,			","},
		{eWhileKW,			"while"},
		{eForKW,			"for"},
		{eOpenCBraces,		"{"},
		{eCloseCBraces,		"}"},
		{eAssignOperator,	"="},
		{eOpenBraces,		"\\["},
		{eCloseBraces,		"]"},
		{eTrueKW,			"true"},
		{eFalseKW,			"false"},
		{eNullKW,			"null"},
		{eIfKW,				"if"},
		{eElseKW,			"else"},
		{eFloat,			"-?[0-9]+.[0-9]+"},
		{eInteger,			"-?[0-9]+"},
		{eString,			"\"[^\"]*\""},
		{eIdentifier,		"_|[a-z]|[A-Z]?_[a-z]|[A-Z]|[0-9]*"},
	};

	CLexer * CRevScriptLexer::sLexer = 0;

	//------------------------------------------------------------------------------------------------------------------
	void CRevScriptLexer::init()
	{
		sLexer = new CLexer(revScriptTokens, sizeof(revScriptTokens)/sizeof(CTokenRule));
	}

	//------------------------------------------------------------------------------------------------------------------
	CLexer * CRevScriptLexer::get()
	{
		return sLexer;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CRevScriptLexer::end()
	{
		delete sLexer;
		sLexer = 0;
	}

}	// namespace script
}	// namespace rev
