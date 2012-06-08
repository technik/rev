////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 26th, 2012
////////////////////////////////////////////////////////////////////////////////
// css lexer

#include "cssLexer.h"

#include <revCore/interpreter/lexer.h>
#include <revCore/interpreter/token.h>
#include <revGame/gui/webPanel/css/cssTokens.h>

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CLexer * CCssFileLexer::sCssFileLexer = 0;
	CLexer * CCssDeclarationLexer::sCssDeclarationLexer = 0;

	//------------------------------------------------------------------------------------------------------------------
	const CTokenRule cssDeclTokenRules[] = 
	{
		{eBackgroundImageProp,	"background-image"},
		{eBackgroundProp,		"background"},
		{eBorderProp,			"border"},
		{eColorProp,			"color"},
		{eFontProp,				"font"},
		{eHeightProp,			"height"},
		{eMarginProp,			"margin"},
		{ePaddingProp,			"padding"},
		{eTextAlignProp,		"text-align"},
		{eWidthProp,			"width"},
		{eOpenParenthesis,		"("},
		{eCloseParenthesis,		")"},
		{eColon,				":"},
		{eSemicolon,			";"},
		{ePixelNumber,			"[0-9]+px"},
		{eInteger,				"[0-9]+"},
		{eHexColor,				"#[0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F][0-9]|[a-f]|[A-F]"},
		{eRgb,					"rgb"},
		{eBlink,				"blink"},
		{eCenter,				"center"},
		{eJustify,				"justify"},
		{eLeft,					"left"},
		{eRight,				"right"},
		{eRed,					"red"},
		{eGreen,				"green"},
		{eBlue,					"blue"},
		{eWhite,				"white"},
		{eBlack,				"black"},
		{eGrey,					"grey"},
		{eYellow,				"yellow"},
		{eOrange,				"orange"},
		{ePurple,				"purple"},
	};

	const CTokenRule cssFileTokenRules[] = {
		{eFileSpace,			" |\n|\r|\f|\t"},
		{eComment,				"/\\*[^*]*\\*/"},
		{eOpenBraces,			"{"},
		{eCloseBraces,			"}"},
		{eComma,				","},
		{eDot,					"."},
		{eHash,					"#"},
		{eDeclarationTk,		"[a-z]+ * \t*: * \t*[^;,:. \t\n\r\f]+;"},
		{eIdentifier,			"[a-z]+"},
	};

	//------------------------------------------------------------------------------------------------------------------
	void CCssFileLexer::init()
	{
		sCssFileLexer = new CLexer(cssFileTokenRules, sizeof(cssFileTokenRules)/sizeof(CTokenRule));
	}

	//------------------------------------------------------------------------------------------------------------------
	CLexer * CCssFileLexer::get()
	{
		return sCssFileLexer;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CCssFileLexer::end()
	{
		delete sCssFileLexer;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CCssDeclarationLexer::init()
	{
		sCssDeclarationLexer = new CLexer(cssDeclTokenRules, sizeof(cssDeclTokenRules)/sizeof(CTokenRule));
	}

	//------------------------------------------------------------------------------------------------------------------
	CLexer * CCssDeclarationLexer::get()
	{
		return sCssDeclarationLexer;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CCssDeclarationLexer::end()
	{
		delete sCssDeclarationLexer;
	}

}	// namespace game
}	// namespace rev
