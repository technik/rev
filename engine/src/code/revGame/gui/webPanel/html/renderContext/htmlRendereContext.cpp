////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 17th, 2012
////////////////////////////////////////////////////////////////////////////////
// html render context

#include "htmlRenderContext.h"

#include <revVideo/font/font.h>
#include <revVideo/texture/texture.h>

using namespace rev::video;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CHtmlRenderContext::CHtmlRenderContext()
	{
	}


	//------------------------------------------------------------------------------------------------------------------
	void CHtmlRenderContext::addText(const char * _text)
	{
		if(!mText.empty())
			mText.append(" ");
		mText.append(_text);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlRenderContext::renderImage(const unsigned char * _srcImg, unsigned _x, unsigned _y, unsigned _width, unsigned _height)
	{
		for(unsigned i = 0; i < _width; ++i)
		{
			if(i+_x >= imgW)
				break;
			for(unsigned j = 0; j < _height; ++j)
			{
				if(j+_y >= imgH)
					break;
				unsigned srcIdx = i + _width * j;
				unsigned dstIdx = i + _x + imgW * (j - _y + imgH - _height);
				dstImg[4*dstIdx+0] = _srcImg[4*srcIdx+0];
				dstImg[4*dstIdx+1] = _srcImg[4*srcIdx+1];
				dstImg[4*dstIdx+2] = _srcImg[4*srcIdx+2];
				dstImg[4*dstIdx+3] = _srcImg[4*srcIdx+3];
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlRenderContext::renderText()
	{
		const char * textToRender = mText.c_str();
		char lineBuffer[512];
		unsigned offset = getTextLine(lineBuffer, textToRender, imgW);
		unsigned yPos = 0;//_y;
		unsigned renderChar0 = 0;
		while(0 != lineBuffer[0])
		{
			CTexture * textTexture = 0;//font->renderText(&lineBuffer[renderChar0]);
			renderChar0 = 0;
			unsigned tWidth = textTexture->width();
			unsigned tHeight = textTexture->height();
			const unsigned char * render = textTexture->buffer();
			renderImage(render, 0, yPos, tWidth, tHeight);
			yPos += tHeight + 4;
			offset += getTextLine(lineBuffer, &textToRender[offset], imgW);
			while(lineBuffer[renderChar0] == ' ')
				++renderChar0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CHtmlRenderContext::getTextLine(char * _dst, const char * _src, unsigned _maxWidth)
	{
		unsigned cursor = 0;
		unsigned wCursor = 0;
		while(_src[cursor] != '\0')
		{
			while((_src[wCursor] != ' ') && (_src[wCursor] != '\t') && (_src[wCursor] != '\0'))
			{
				_dst[wCursor] = _src[wCursor];
				++wCursor;
			}
			_dst[wCursor] = '\0';
			unsigned width = 0;//font->textLength(_dst);
			_dst[wCursor] = _src[wCursor];
			if(width > _maxWidth)
				break;
			cursor = wCursor++;
		}
		_dst[cursor] = '\0';
		return cursor;
	}

}	// namespace game
}	// namespace rev