////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 17th, 2012
////////////////////////////////////////////////////////////////////////////////
// html expressions

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_HTMLEXPRESSIONS_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_HTMLEXPRESSIONS_H_

namespace rev { namespace game
{
	//--- Html syntax definition ---
	enum EHtmlSyntax // Non-terminal ids
	{
		eDocument,
		eHead,
		eHeadCode,
		eMetadata,
		eBody,
		eArgList,
		eArgument,
		eAttribute,
		eHtmlElement,
		eTag,
		eHtmlCode
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_HTMLEXPRESSIONS_H_