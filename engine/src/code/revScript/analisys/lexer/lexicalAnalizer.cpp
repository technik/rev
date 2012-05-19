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
		{eCommentToken,	"//[^\n\r\f]"},
		{eSpace,		"\t| |\n|\r|\f+"},
		{eFloat,		"-?[0-9]+.[0-9]+"},
		{eInteger,		"-?[0-9]+"},
		{eString,		"\"[^\"]*\""},
		{eTrue,			"true"},
		{eFalse,		"false"},
		{eOpenBraces,	"\\["},
		{eCloseBraces,	"]"},
		{eComma,		","},
		{eSemicolon,	";"},
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
