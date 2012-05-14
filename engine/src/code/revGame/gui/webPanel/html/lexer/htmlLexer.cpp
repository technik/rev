////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// html lexer

#include "htmlLexer.h"

#include <revCore/interpreter/lexer.h>
#include <revCore/interpreter/token.h>
#include <revGame/gui/webPanel/html/htmlTokens.h>

namespace rev { namespace game
{
	const CTokenRule htmlLexRules[] = {
		{eOpenEndTag,	"</"},
		{eOpenTag,		"<"},
		{eCloseTag,		"/?>"},
		{eAssignement,	"="},
		{ePercentage,	"%"},
		{eSpace,		" |\t|\n|\r|\f+"},
		{eInt,			"-?[0-9]+"},
		{eFloat,		"-?[0-9]+.[0-9]*f?"},
		{eColor,		"#[0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F]"},
		{eId,			"[a-z]+"},
		{eWord,			"[^ \t\n\r\f<"},
		{eString,		"\"[^\"]*"},
	};

	CLexer * CHtmlLexer::sHtmlLexer = 0;

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlLexer::init()
	{
		sHtmlLexer = new CLexer(htmlLexRules, sizeof(htmlLexRules)/sizeof(CTokenRule));
	}

	//------------------------------------------------------------------------------------------------------------------
	CLexer * CHtmlLexer::get()
	{
		return sHtmlLexer;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlLexer::end()
	{
		delete sHtmlLexer;
	}
}	// namespace game
}	// namespace rev
