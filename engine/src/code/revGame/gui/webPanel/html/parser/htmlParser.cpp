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
	CSyntagma document			= { eDocument , false };
	CSyntagma head				= { eHead, false };
	CSyntagma headCode			= { eHeadCode, false };
	CSyntagma metadata			= { eMetadata, false };
	CSyntagma body				= { eBody, false };
	CSyntagma argList			= { eArgList, false };
	CSyntagma argument			= { eArgument, false };
	CSyntagma attribute			= { eAttribute, false };
	CSyntagma htmlElement		= { eHtmlElement, false };
	CSyntagma tag				= { eTag, false };
	CSyntagma htmlCode			= { eHtmlCode, false };
	// Terminals
	CSyntagma openHtml			= { eOpenHtml, true };
	CSyntagma closeHtml			= { eCloseHtml, true };
	CSyntagma openHead			= { eOpenHead, true };
	CSyntagma closeHead			= { eCloseHead, true };
	CSyntagma linkTag			= { eLinkTag, true };
	CSyntagma bodyTag			= { eBodyTag, true };
	CSyntagma divTag			= { eDivTag, true };
	CSyntagma spanTag			= { eSpanTag, true };
	CSyntagma h1Tag				= { eH1Tag, true };
	CSyntagma h2Tag				= { eH2Tag, true };
	CSyntagma h3Tag				= { eH3Tag, true };
	CSyntagma h4Tag				= { eH4Tag, true };
	CSyntagma h5Tag				= { eH5Tag, true };
	CSyntagma h6Tag				= { eH6Tag, true };
	CSyntagma BrTag				= { eBrTag, true };
	CSyntagma imgTag			= { eImgTag, true };
	CSyntagma classAttrib		= { eClassAttrib, true };
	CSyntagma hiddenAttrib		= { eHiddenAttrib, true };
	CSyntagma hrefAttrib		= { eHrefAttrib, true };
	CSyntagma idAttrib			= { eIdAttrib, true };
	CSyntagma styleAttrib		= { eStyleAttrib, true };
	CSyntagma openEndTag		= { eOpenEndTag, true };
	CSyntagma openTag			= { eOpenTag, true };
	CSyntagma closeTag			= { eCloseTag, true };
	CSyntagma assignement		= { eAssignement, true };
	CSyntagma stringSynt		= { eString, true };
	CSyntagma word				= { eWord, true };

	// -- Html syntax rules
	CGrammarRule htmlSyntaxRules[19];

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlParser::init()
	{
		// Document : OpenHtml Head Body CloseHtml
		htmlSyntaxRules[0].from = eDocument;
		htmlSyntaxRules[0].to.push_back(openHtml);
		htmlSyntaxRules[0].to.push_back(head);
		htmlSyntaxRules[0].to.push_back(body);
		htmlSyntaxRules[0].to.push_back(closeHtml);
		// Head : OpenHead headContent CloseHead
		htmlSyntaxRules[1].from = eHead;
		htmlSyntaxRules[1].to.push_back(openHead);
		htmlSyntaxRules[1].to.push_back(headCode);
		htmlSyntaxRules[1].to.push_back(closeHead);
		// Head :
		htmlSyntaxRules[2].from = eHead;
		// HeadCode : Metadata HeadCode
		htmlSyntaxRules[3].from = eHeadCode;
		htmlSyntaxRules[3].to.push_back(metadata);
		htmlSyntaxRules[3].to.push_back(headCode);
		// HeadCode :
		htmlSyntaxRules[4].from = eHeadCode;
		// Metadata : <link href=string>
		htmlSyntaxRules[5].from = eMetadata;
		htmlSyntaxRules[5].to.push_back(openTag);
		htmlSyntaxRules[5].to.push_back(linkTag);
		htmlSyntaxRules[5].to.push_back(hrefAttrib);
		htmlSyntaxRules[5].to.push_back(assignement);
		htmlSyntaxRules[5].to.push_back(stringSynt);
		htmlSyntaxRules[5].to.push_back(closeTag);
		// Body : <body argList> htmlCode </body>
		htmlSyntaxRules[6].from = eBody;
		htmlSyntaxRules[6].to.push_back(openTag);
		htmlSyntaxRules[6].to.push_back(bodyTag);
		htmlSyntaxRules[6].to.push_back(argList);
		htmlSyntaxRules[6].to.push_back(closeTag);
		htmlSyntaxRules[6].to.push_back(htmlCode);
		htmlSyntaxRules[6].to.push_back(openEndTag);
		htmlSyntaxRules[6].to.push_back(bodyTag);
		htmlSyntaxRules[6].to.push_back(closeTag);
		// argList : argument argList
		htmlSyntaxRules[7].from = eArgList;
		htmlSyntaxRules[7].to.push_back(argument);
		htmlSyntaxRules[7].to.push_back(argList);
		// argList :
		htmlSyntaxRules[8].from = eArgList;
		// argument : attrib=string
		htmlSyntaxRules[9].from = eArgument;
		htmlSyntaxRules[9].to.push_back(attribute);
		htmlSyntaxRules[9].to.push_back(assignement);
		htmlSyntaxRules[9].to.push_back(stringSynt);
		// attrib : styleAttrib
		htmlSyntaxRules[10].from = eAttribute;
		htmlSyntaxRules[10].to.push_back(styleAttrib);
		// attrib : classAttrib
		htmlSyntaxRules[11].from = eAttribute;
		htmlSyntaxRules[11].to.push_back(classAttrib);
		// attrib : idAttrib
		htmlSyntaxRules[12].from = eAttribute;
		htmlSyntaxRules[12].to.push_back(idAttrib);
		// HtmlCode : Word HtmlCode
		htmlSyntaxRules[13].from = eHtmlCode;
		htmlSyntaxRules[13].to.push_back(word);
		htmlSyntaxRules[13].to.push_back(htmlCode);
		// HtmlCode : htmlElement HtmlCode
		htmlSyntaxRules[14].from = eHtmlCode;
		htmlSyntaxRules[14].to.push_back(htmlElement);
		htmlSyntaxRules[14].to.push_back(htmlCode);
		// HtmlCode :
		htmlSyntaxRules[15].from = eHtmlCode;
		// HtmlElement : <tag argList> htmlCode </tag>
		htmlSyntaxRules[16].from = eHtmlElement;
		htmlSyntaxRules[16].to.push_back(openTag);
		htmlSyntaxRules[16].to.push_back(tag);
		htmlSyntaxRules[16].to.push_back(argList);
		htmlSyntaxRules[16].to.push_back(closeTag);
		htmlSyntaxRules[16].to.push_back(htmlCode);
		htmlSyntaxRules[16].to.push_back(openEndTag);
		htmlSyntaxRules[16].to.push_back(tag);
		htmlSyntaxRules[16].to.push_back(closeTag);
		// Tag : divTag
		htmlSyntaxRules[17].from = eTag;
		htmlSyntaxRules[17].to.push_back(divTag);
		// Tag : spanTag
		htmlSyntaxRules[18].from = eTag;
		htmlSyntaxRules[18].to.push_back(spanTag);

		// Create the parser
		sHtmlParser = new CParser(htmlSyntaxRules, 19);
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
