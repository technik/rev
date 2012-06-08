////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 27th, 2012
////////////////////////////////////////////////////////////////////////////////
// css expressions

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_HTMLEXPRESSIONS_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_HTMLEXPRESSIONS_H_

namespace rev { namespace game
{
	//--- Html syntax definition ---
	enum ECssDeclSyntax // Non-terminal ids
	{
		eDeclaration,
		eCssDecl,
		eNumericDecl,
		eColorDecl,
		eAlignDecl,
		eGenericDecl,
		eNumericProperty,
		eColorProperty,
		eAlignProperty,
		eGenericProperty,
		eColorValue,
		eAlignValue,
		eNumericValue,
	};

	enum ECssFileSyntax
	{
		eDocument,
		eRuleList,
		eRule,
		eSelector,
		eDeclList,
		eDeclBlock,
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_HTMLEXPRESSIONS_H_