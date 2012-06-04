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
	const CTokenRule htmlLexRules[] = 
	{
		// Core html tokens
		{eOpenHtml,		"<html>"},
		{eCloseHtml,	"</html>"},
		{eOpenHead,		"<head>"},
		{eCloseHead,	"</head>"},
		// Metadata tags
		{eLinkTag,		"link"},
		// Other tags
		{eBodyTag,		"body"},
		{ePTag,			"p"},
		{eH1Tag,		"h1"},
		{eH2Tag,		"h2"},
		{eH3Tag,		"h3"},
		{eH4Tag,		"h6"},
		{eH5Tag,		"h5"},
		{eH6Tag,		"h6"},
		{eImgTag,		"img"},
		// Attributes
		{eClassAttrib,	"class"},
		{eHiddenAttrib,	"hidden"},
		{eHrefAttrib,	"href"},
		{eIdAttrib,		"id"},
		{eStyleAttrib,	"style"},
		// Generic tokens
		{eComment,		"<!--[^-]*-->"},
		{eOpenEndTag,	"</"},
		{eOpenTag,		"<"},
		{eCloseTag,		">"},
		{eSpace,		" |\t|\n|\r|\f+"},
		{eAssignement,	"="},
		{eString,		"\"[^\"\n\r\f]*\""},
		{eWord,			"[^ \t\n\r\f<]+"},
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
