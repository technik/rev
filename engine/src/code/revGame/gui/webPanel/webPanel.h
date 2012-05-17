////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 11th, 2012
////////////////////////////////////////////////////////////////////////////////
// web-based panel

#ifndef _REV_GAME_GUI_WEBPANEL_WEBPANEL_H_
#define _REV_GAME_GUI_WEBPANEL_WEBPANEL_H_

#include "html/domTree/htmlDomTree.h"
#include <revCore/delegate/delegate.h>
#include <revGame/gui/guiPanel.h>

namespace rev { namespace video { class CFont; }}

namespace rev { namespace game
{
	class CWebPanel : public CGuiPanel
	{
	public:
		CWebPanel(unsigned width, unsigned height);
		~CWebPanel();

		void	loadPage(const char * _pageName);
		void	runHtml(const char * _code);
	private:
		void clearImage(unsigned char * _img);
		void renderCode(unsigned char * _dstImg, const char * _code);

	private:
		unsigned mWidth;
		unsigned mHeight;
		video::CFont * mDefaultFont;
		CObjectDelegate<CWebPanel,const char*> * mPageDelegate;
		CHtmlDomNode * mTree;
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_WEBPANEL_H_