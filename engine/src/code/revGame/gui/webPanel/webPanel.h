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
		void renderText(unsigned char * _dstImg, const char * _text, unsigned _x, unsigned _y);
		void renderImage(unsigned char * _dstImg, const unsigned char * _srcImg, unsigned _x, unsigned _y, unsigned width, unsigned height);
		unsigned getTextLine(char * _dst, const char * _src, unsigned _maxWidth);

	private:
		unsigned mWidth;
		unsigned mHeight;
		video::CFont * mDefaultFont;
		CObjectDelegate<CWebPanel,const char*> * mPageDelegate;
		//CHtmlDomTree	mDomTree;
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_WEBPANEL_H_