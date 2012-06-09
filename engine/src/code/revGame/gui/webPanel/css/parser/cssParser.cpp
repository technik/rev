////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 27th, 2012
////////////////////////////////////////////////////////////////////////////////
// css parsers

#include "cssParser.h"
#include <revGame/gui/webPanel/css/cssExpressions.h>
#include <revGame/gui/webPanel/css/cssTokens.h>

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CParser * CCssDeclParser::sCssDeclParser = 0;
	CParser * CCssFileParser::sCssFileParser = 0;

	//------------------------------------------------------------------------------------------------------------------
	// Declaration syntagmas
	CSyntagma dcDeclaration			= { eDeclaration, false };
	CSyntagma dcCssDecl				= { eCssDecl, false };
	CSyntagma dcColorDecl			= { eColorDecl, false };
	CSyntagma dcAlignDecl			= { eAlignDecl, false };
	CSyntagma dcNumericDecl			= { eNumericDecl, false };
	CSyntagma dcGenericDecl			= { eGenericDecl, false };
	CSyntagma dcNumericProperty		= { eNumericProperty, false };
	CSyntagma dcColorProperty		= { eColorProperty, false };
	CSyntagma dcAlignProperty		= { eAlignProperty, false };
	CSyntagma dcGenericProperty		= { eGenericProperty, false };
	CSyntagma dcNumericValue		= { eNumericValue, false };
	CSyntagma dcAlignValue			= { eAlignValue, false };
	CSyntagma dcColorValue			= { eColorValue, false };
	CSyntagma dcBackgroundImageProp	= { eBackgroundImageProp, true };
	CSyntagma dcBackgroundProp		= { eBackgroundProp, true };
	CSyntagma dcBorderProp			= { eBorderProp, true };
	CSyntagma dcColorProp			= { eColorProp, true };
	CSyntagma dcFontProp			= { eFontProp, true };
	CSyntagma dcHeightProp			= { eHeightProp, true };
	CSyntagma dcPaddingProp			= { ePaddingProp, true };
	CSyntagma dcTextAlignProp		= { eTextAlignProp, true };
	CSyntagma dcWidthProp			= { eWidthProp, true };
	CSyntagma dcOpenParenthesis		= { eOpenParenthesis, true };
	CSyntagma dcCloseParenthesis	= { eCloseParenthesis, true };
	CSyntagma dcColon				= { eColon,	true };
	CSyntagma dcSemicolon			= { eSemicolon, true };
	CSyntagma dcPixelNumber			= { ePixelNumber, true };
	CSyntagma dcInteger				= { eInteger, true };
	CSyntagma dcHexColor			= { eHexColor, true };
	CSyntagma dcRgb					= { eRgb, true };
	CSyntagma dcBlink				= { eBlink, true };
	CSyntagma dcCenter				= { eCenter, true };
	CSyntagma dcJustify				= { eJustify, true };
	CSyntagma dcLeft				= { eLeft, true };
	CSyntagma dcRight				= { eRight, true };
	CSyntagma dcRed					= { eRed, true };
	CSyntagma dcGreen				= { eGreen, true };
	CSyntagma dcBlue				= { eBlue, true };
	CSyntagma dcWhite				= { eWhite, true };
	CSyntagma dcBlack				= { eBlack, true };
	CSyntagma dcGrey				= { eGrey, true };
	CSyntagma dcYellow				= { eYellow, true };
	CSyntagma dcOrange				= { eOrange, true };
	CSyntagma dcPurple				= { ePurple, true };

	// File syntagmas
	CSyntagma flDocument			= { eDocument, false };
	CSyntagma flRuleList			= { eRuleList, false };
	CSyntagma flRule				= { eRule, false };
	CSyntagma flSelector			= { eSelector, false };
	CSyntagma flDeclList			= { eDeclList, false };
	CSyntagma flDeclBlock			= { eDeclBlock, false };
	CSyntagma flOpenBraces			= { eOpenBraces, true };
	CSyntagma flCloseBraces			= { eCloseBraces, true };
	CSyntagma flComma				= { eComma, true };
	CSyntagma flDot					= { eDot, true };
	CSyntagma flHash				= { eHash, true };
	CSyntagma flIdentifier			= { eIdentifier, true };
	CSyntagma flDeclaration			= { eDeclarationTk, true };

	// -- Html syntax rules
	CGrammarRule cssDeclSyntaxRules[14];
	//CGrammarRule cssFileSyntaxRules[1];

	//------------------------------------------------------------------------------------------------------------------
	void CCssDeclParser::init()
	{
		// Declaration : cssDecl
		cssDeclSyntaxRules[0].from = eDeclaration;
		cssDeclSyntaxRules[0].to.push_back(dcCssDecl);
		// cssDecl : colorDecl
		cssDeclSyntaxRules[1].from = eCssDecl;
		cssDeclSyntaxRules[1].to.push_back(dcColorDecl);
		// cssDecl : alignDecl
		cssDeclSyntaxRules[2].from = eCssDecl;
		cssDeclSyntaxRules[2].to.push_back(dcAlignDecl);
		// cssDecl : numericDecl
		cssDeclSyntaxRules[3].from = eCssDecl;
		cssDeclSyntaxRules[3].to.push_back(dcNumericDecl);
		// cssDecl : genericDecl
		cssDeclSyntaxRules[4].from = eCssDecl;
		cssDeclSyntaxRules[4].to.push_back(dcGenericDecl);
		// colorDecl : colorProperty colon colorValue semicolon
		cssDeclSyntaxRules[5].from = eColorDecl;
		cssDeclSyntaxRules[5].to.push_back(dcColorProperty);
		cssDeclSyntaxRules[5].to.push_back(dcColon);
		cssDeclSyntaxRules[5].to.push_back(dcColorValue);
		cssDeclSyntaxRules[5].to.push_back(dcSemicolon);
		// colorProperty : background
		cssDeclSyntaxRules[6].from = eColorProperty;
		cssDeclSyntaxRules[6].to.push_back(dcBackgroundProp);
		// colorProperty : color
		cssDeclSyntaxRules[7].from = eColorProperty;
		cssDeclSyntaxRules[7].to.push_back(dcColorProp);
		// colorValue : hexColor
		cssDeclSyntaxRules[8].from = eColorValue;
		cssDeclSyntaxRules[8].to.push_back(dcHexColor);
		// colorValue : red
		cssDeclSyntaxRules[9].from = eColorValue;
		cssDeclSyntaxRules[9].to.push_back(dcRed);
		// colorValue : green
		cssDeclSyntaxRules[10].from = eColorValue;
		cssDeclSyntaxRules[10].to.push_back(dcGreen);
		// colorValue : blue
		cssDeclSyntaxRules[11].from = eColorValue;
		cssDeclSyntaxRules[11].to.push_back(dcBlue);
		// colorValue : white
		cssDeclSyntaxRules[12].from = eColorValue;
		cssDeclSyntaxRules[12].to.push_back(dcWhite);
		// colorValue : black
		cssDeclSyntaxRules[13].from = eColorValue;
		cssDeclSyntaxRules[13].to.push_back(dcBlack);
		// colorValue : rgb ( integer, integer, integer )
		// Create the parser
		sCssDeclParser = new CParser(cssDeclSyntaxRules, 14);
	}

	//------------------------------------------------------------------------------------------------------------------
	CParser * CCssDeclParser::get()
	{
		return sCssDeclParser;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CCssDeclParser::end()
	{
		delete sCssDeclParser;
	}

}	// namespace game
}	// namespace rev