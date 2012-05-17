////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 15th, 2012
////////////////////////////////////////////////////////////////////////////////
// Html parser

#include "htmlParser.h"
#include <revGame/gui/webPanel/html/htmlExpressions.h>
#include <revGame/gui/webPanel/html/htmlTokens.h>

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CParser * CHtmlParser::sHtmlParser = 0;

	
	// -- Html syntagmas --
	// Non terminals
	CSyntagma document	= { eDocument , false };
	CSyntagma body		= { eBody, false };
	CSyntagma htmlCode	= { eHtmlCode, false };
	// Terminals
	CSyntagma openHtml	= { eOpenHtml, true };
	CSyntagma closeHtml	= { eCloseHtml, true };
	CSyntagma openBody	= { eOpenBody, true };
	CSyntagma closeBody = { eCloseBody, true };
	CSyntagma word		= { eWord, true };

	// -- Html syntax rules
	CGrammarRule htmlSyntaxRules[4];

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlParser::init()
	{
		// Document : OpenHtml Body CloseHtml
		htmlSyntaxRules[0].from = eDocument;
		htmlSyntaxRules[0].to.push_back(openHtml);
		htmlSyntaxRules[0].to.push_back(body);
		htmlSyntaxRules[0].to.push_back(closeHtml);
		// Body : OpenBody htmlCode CloseBody
		htmlSyntaxRules[1].from = eBody;
		htmlSyntaxRules[1].to.push_back(openBody);
		htmlSyntaxRules[1].to.push_back(htmlCode);
		htmlSyntaxRules[1].to.push_back(closeBody);
		// HtmlCode : Word HtmlCode
		htmlSyntaxRules[2].from = eHtmlCode;
		htmlSyntaxRules[2].to.push_back(word);
		htmlSyntaxRules[2].to.push_back(htmlCode);
		// HtmlCode :
		htmlSyntaxRules[3].from = eHtmlCode;

		// Create the parser
		sHtmlParser = new CParser(htmlSyntaxRules, 4);
	}

	//------------------------------------------------------------------------------------------------------------------
	CParser * CHtmlParser::get()
	{
		return sHtmlParser;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlParser::end()
	{
		delete sHtmlParser;
	}

}	// namespace rev
}	// namespace game
