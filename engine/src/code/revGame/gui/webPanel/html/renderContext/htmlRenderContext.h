////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 17th, 2012
////////////////////////////////////////////////////////////////////////////////
// html render context

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_RENDERCONTEXT_HTMLRENDERCONTEXT_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_RENDERCONTEXT_HTMLRENDERCONTEXT_H_

#include <revCore/string.h>

namespace rev { namespace video { class CFont; }}

namespace rev { namespace game
{
	class CHtmlRenderContext
	{
	public:
		CHtmlRenderContext();

		unsigned char * dstImg;
		unsigned imgW;
		unsigned imgH;
		video::CFont * font;

		void addText(const char * _text);
		void renderText(unsigned _x, unsigned _y);

	private:
		unsigned getTextLine(char * _dst, const char * _src, unsigned _maxWidth);
		void renderImage(const unsigned char * _srcImg, unsigned _x, unsigned _y, unsigned _width, unsigned _height);

	private:
		string	mText;
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_RENDERCONTEXT_HTMLRENDERCONTEXT_H_
