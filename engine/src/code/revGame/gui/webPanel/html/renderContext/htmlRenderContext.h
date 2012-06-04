////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 17th, 2012
////////////////////////////////////////////////////////////////////////////////
// html render context

#ifndef _REV_GAME_GUI_WEBPANEL_HTML_RENDERCONTEXT_HTMLRENDERCONTEXT_H_
#define _REV_GAME_GUI_WEBPANEL_HTML_RENDERCONTEXT_HTMLRENDERCONTEXT_H_

#include <revCore/string.h>
#include <revVideo/color/color.h>
#include <vector.h>

namespace rev { namespace video { class CFont; }}

namespace rev { namespace game
{
	class CCssDeclaration;

	struct CHtmlRenderState
	{
		CHtmlRenderState()
			:mColor(video::CColor::WHITE)
			,mBgColor(0.f,0.f,0.f,0.f)
		{}
		video::CColor	mColor;
		video::CColor	mBgColor;
		video::CFont*	mFont;
		unsigned		mWidth;
		unsigned		mHeight;
		unsigned		mX0;
		unsigned		mY0;
	};

	class CHtmlRenderContext
	{
	public:
		CHtmlRenderContext();

		unsigned char * dstImg;
		unsigned imgW;
		unsigned imgH;

		void addText(const char * _text);
		void renderText();
		void addStyle(const CCssDeclaration& _style);
		void removeTopStyle();

	private:
		unsigned getTextLine(char * _dst, const char * _src, unsigned _maxWidth);
		void renderImage(const unsigned char * _srcImg, unsigned _x, unsigned _y, unsigned _width, unsigned _height);

	private:
		string	mText;
		rtl::vector<CHtmlRenderState>	mStateStack;
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_GUI_WEBPANEL_HTML_RENDERCONTEXT_HTMLRENDERCONTEXT_H_
