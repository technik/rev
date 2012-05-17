////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 17th, 2012
////////////////////////////////////////////////////////////////////////////////
// html Dom Tree

#include "htmlDomTree.h"

#include <revCore/codeTools/assert/assert.h>
#include <revCore/codeTools/log/log.h>
#include <revCore/interpreter/parser.h>
#include <revCore/interpreter/token.h>
#include <revGame/gui/webPanel/html/htmlTokens.h>
#include <revGame/gui/webPanel/html/htmlExpressions.h>
#include <revVideo/font/font.h>
#include <revVideo/texture/texture.h>

using namespace rev::video;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CHtmlDomNode * CHtmlDomNode::createNode(CParserNode * _node)
	{
		_node;
		return new CHtmlDomNode();
	}

	//------------------------------------------------------------------------------------------------------------------
	void CHtmlDomNode::render(unsigned char * _dstImg, unsigned _w, unsigned _h, unsigned _x, unsigned _y, CFont * _font) const
	{
		for(unsigned i = 0; i < mChildren.size(); ++i)
		{
			mChildren[i]->render(_dstImg, _w, _h, _x, _y, _font);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void renderImage(unsigned char * _dstImg, unsigned _dstW, unsigned _dstH, const unsigned char * _srcImg, unsigned _x, unsigned _y, unsigned _width, unsigned _height)
	{
		for(unsigned i = 0; i < _width; ++i)
		{
			if(i+_x >= _dstW)
				break;
			for(unsigned j = 0; j < _height; ++j)
			{
				if(j+_y >= _dstH)
					break;
				unsigned srcIdx = i + _width * j;
				unsigned dstIdx = i + _x + _dstW * (j - _y + _dstH - _height);
				_dstImg[4*dstIdx+0] = _srcImg[4*srcIdx+0];
				_dstImg[4*dstIdx+1] = _srcImg[4*srcIdx+1];
				_dstImg[4*dstIdx+2] = _srcImg[4*srcIdx+2];
				_dstImg[4*dstIdx+3] = _srcImg[4*srcIdx+3];
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	// void CHtmlCodeDomNode::render(unsigned char * _dstImg, unsigned _w, unsigned _h, unsigned _x, unsigned _y, CFont * _font) const
	// {
	// 	// Render text
	// 	CTexture * textTexture = _font->renderText(mToken.text);
	// 	unsigned tWidth = textTexture->width();
	// 	unsigned tHeight = textTexture->height();
	// 	const unsigned char * render = textTexture->buffer();
	// 	renderImage(_dstImg, _w, _h, render, _x, _y, tWidth, tHeight);
	// 	// Render children
	// 	CHtmlDomNode::render(_dstImg, _w, _h, _x+tWidth+_font->chars()[' '].xadvance, _y, _font);
	// }

}	// namespace game
}	// namespace rev