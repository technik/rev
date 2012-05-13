////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 11th, 2012
////////////////////////////////////////////////////////////////////////////////
// web-based panel

#include "webPanel.h"
#include "html/htmlTokens.h"
#include "html/lexer/htmlLexer.h"

#include <revCore/file/file.h>
#include <revCore/file/fileWatcher.h>
#include <revVideo/font/font.h>
#include <revVideo/texture/texture.h>
#include <vector.h>

using namespace rev::video;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CWebPanel::CWebPanel(unsigned _width, unsigned _height)
		:CGuiPanel((CTexture*)0)
		,mWidth(_width)
		,mHeight(_height)
		,mPageDelegate(0)
	{
		// Load fonts
		mDefaultFont = CFont::get("arial32.fnt");
	}

	//------------------------------------------------------------------------------------------------------------------
	CWebPanel::~CWebPanel()
	{
		mDefaultFont->release();
		if(0 != mPageDelegate)
			delete mPageDelegate;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CWebPanel::loadPage(const char * _pageName)
	{
		CFile pageFile(_pageName);
		runHtml(pageFile.textBuffer());
		// Set a file watcher
		mPageDelegate = new CObjectDelegate<CWebPanel, const char*>(this, &CWebPanel::loadPage);
		SFileWatcher::get()->addWatcher(_pageName, mPageDelegate);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CWebPanel::runHtml(const char * _code)
	{
		if(0 != mPageDelegate)
		{
			SFileWatcher::get()->removeWatcher(mPageDelegate);
			delete mPageDelegate;
			mPageDelegate = 0;
		}
		// Initialize the image
		unsigned imgSize = 4 * mWidth * mHeight;
		unsigned char * img = new unsigned char[imgSize];
		clearImage(img);

		// render code
		renderCode(img, _code);

		// Set the image as a texture
		CTexture * texture = new CTexture(img, mWidth, mHeight);
		CGuiPanel::setTexture(texture);
		texture->release(); // So CGuiPanel takes care of texture ownership
	}

	//------------------------------------------------------------------------------------------------------------------
	void CWebPanel::clearImage(unsigned char * _img)
	{
		for(unsigned i = 0; i < mWidth; ++i)
		{
			for(unsigned j = 0; j < mHeight; ++j)
			{
				unsigned idx = i + mWidth * j;
				// XOR texture
				_img[4*idx+0] = 0; //(unsigned char)((i^j) & 0xff);
				_img[4*idx+1] = 0; //(unsigned char)((i^j) & 0xff);
				_img[4*idx+2] = 0; //(unsigned char)((i^j) & 0xff);
				_img[4*idx+3] = 0; //255;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CWebPanel::renderCode(unsigned char * _dstImg, const char * _code)
	{
		rtl::vector<CHtmlToken>	tokenList;
		// Parse
		CHtmlLexer::parseHtmlIntoTokens(_code, tokenList);
		// Load
		mDomTree.loadFromTokenList(tokenList);
		// Render
		mDomTree.renderToImage(_dstImg, mWidth, mHeight);
		//renderText(_dstImg, _code, 0, 0);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CWebPanel::renderText(unsigned char * _dstImg, const char * _text, unsigned _x, unsigned _y)
	{
		char lineBuffer[512];
		unsigned offset = getTextLine(lineBuffer, _text, mWidth);
		unsigned yPos = _y;
		unsigned renderChar0 = 0;
		while(0 != lineBuffer[0])
		{
			CTexture * textTexture = mDefaultFont->renderText(&lineBuffer[renderChar0]);
			renderChar0 = 0;
			unsigned tWidth = textTexture->width();
			unsigned tHeight = textTexture->height();
			const unsigned char * render = textTexture->buffer();
			renderImage(_dstImg, render, _x, yPos, tWidth, tHeight);
			yPos += tHeight + 4;
			offset += getTextLine(lineBuffer, &_text[offset], mWidth);
			while(lineBuffer[renderChar0] == ' ')
				++renderChar0;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CWebPanel::renderImage(unsigned char * _dstImg, const unsigned char * _srcImg, unsigned _x, unsigned _y, unsigned _width, unsigned _height)
	{
		for(unsigned i = 0; i < _width; ++i)
		{
			if(i+_x >= mWidth)
				break;
			for(unsigned j = 0; j < _height; ++j)
			{
				if(j+_y >= mHeight)
					break;
				unsigned srcIdx = i + _width * j;
				unsigned dstIdx = i + _x + mWidth * (j - _y + mHeight - _height);
				_dstImg[4*dstIdx+0] = _srcImg[4*srcIdx+0];
				_dstImg[4*dstIdx+1] = _srcImg[4*srcIdx+1];
				_dstImg[4*dstIdx+2] = _srcImg[4*srcIdx+2];
				_dstImg[4*dstIdx+3] = _srcImg[4*srcIdx+3];
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CWebPanel::getTextLine(char * _dst, const char * _src, unsigned _maxWidth)
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
			unsigned width = mDefaultFont->textLength(_dst);
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
