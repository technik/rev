////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 14th, 2012
////////////////////////////////////////////////////////////////////////////////
// html tokens

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_HTMLTOKENS_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_HTMLTOKENS_H_

namespace rev { namespace game
{
	enum EHtmlToken
	{
		// Core html tokens
		eOpenHtml,
		eCloseHtml,
		eOpenHead,
		eCloseHead,
		// Metadata tags
		eLinkTag,
		// Other tags
		eBodyTag,
		eBrTag,
		eDivTag,
		eImgTag,
		eH1Tag,
		eH2Tag,
		eH3Tag,
		eH4Tag,
		eH5Tag,
		eH6Tag,
		ePTag,
		eSpanTag,
		// Attributes
		eClassAttrib,
		eHiddenAttrib,
		eHrefAttrib,
		eIdAttrib,
		eStyleAttrib,
		// Generic tokens
		eComment,
		eOpenEndTag,
		eOpenTag,
		eCloseTag,
		eSpace,
		eAssignement,
		eString,
		eWord
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_HTMLTOKENS_H_
