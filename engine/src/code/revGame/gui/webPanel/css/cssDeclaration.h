////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 27th, 2012
////////////////////////////////////////////////////////////////////////////////
// css declaration

#ifndef _REV_GAME_GUI_WEBPANEL_CSS_CSSDECLARATION_H_
#define _REV_GAME_GUI_WEBPANEL_CSS_CSSDECLARATION_H_

#include <revVideo/color/color.h>

namespace rev { class CParserNode; }

namespace rev { namespace game
{
	class CCssDeclaration
	{
	public:
		CCssDeclaration(const char * _code);

	public:
		bool hasColor;
		video::CColor	color;
		bool hasBgColor;
		video::CColor	bgColor;

	private:
		void processTree(CParserNode * _tree);
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_CSS_CSSDECLARATION_H_