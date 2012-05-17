////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// html lexer

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_HTMLTOKENS_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_HTMLTOKENS_H_

namespace rev { namespace game
{
	enum EHtmlToken
	{
		eSpace,
		eOpenHtml,
		eCloseHtml,
		eOpenBody,
		eCloseBody,
		eWord
	};
	/*enum EHtmlToken
	{
		eOpenEndTag,
		eOpenTag,
		eCloseTag,
		eAssignement,
		ePercentage,
		eSpace,
		eInt,
		eFloat,
		eId,
		eColor,
		eWord,
		eString,
	};*/
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_HTMLTOKENS_H_
