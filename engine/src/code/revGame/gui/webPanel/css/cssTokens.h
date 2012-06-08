////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 26th, 2012
////////////////////////////////////////////////////////////////////////////////
// css tokens

#ifndef _REV_GAME_GUI_WEBPANEL_CSS_CSSTOKENS_H_
#define _REV_GAME_GUI_WEBPANEL_CSS_CSSTOKENS_H_

namespace rev { namespace game
{
	enum ECssDeclarationTokens
	{
		eDecSpace,
		// Properties
		eBackgroundImageProp,
		eBackgroundProp,
		eBorderProp,
		eColorProp,
		eFontProp,
		eHeightProp,
		eMarginProp,
		ePaddingProp,
		eTextAlignProp,
		eWidthProp,
		// Punctuation
		eOpenParenthesis,		// (
		eCloseParenthesis,		// )
		eColon,					// :
		eSemicolon,				// ;
		// Numbers and literals
		ePixelNumber,
		eInteger,
		eHexColor,
		eRgb,
		// Property values
		eBlink,
		eCenter,
		eJustify,
		eLeft,
		eRight,
		// Common colors
		eRed,
		eGreen,
		eBlue,
		eWhite,
		eBlack,
		eGrey,
		eYellow,
		eOrange,
		ePurple,
	};

	enum ECssFileTokens
	{
		eFileSpace,
		eComment,
		// Punctuation
		eOpenBraces,			// {
		eCloseBraces,			// }
		eComma,					// ,
		eDot,					// .
		eHash,					// #
		// General css
		eIdentifier,
		eDeclarationTk
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_CSS_CSSTOKENS_H_
