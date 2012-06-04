////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 11th, 2012
////////////////////////////////////////////////////////////////////////////////
// web-based panel

#include "webPanel.h"
#include "html/domTree/htmlDomTree.h"
#include "html/lexer/htmlLexer.h"
#include "html/parser/htmlParser.h"
#include "html/renderContext/htmlRenderContext.h"
#include "css/cssDeclaration.h"

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
		_dstImg;
		rtl::vector<CToken>	tokenList;
		// Parse
		CHtmlLexer::get()->tokenizeCode(tokenList, _code);	// Lexycal analysis
		CParser * htmlParser = CHtmlParser::get();
		htmlParser->stripTokens(tokenList, eSpace);			// Strip spaces
		htmlParser->stripTokens(tokenList, eComment);
		CParserNode * htmlTree = htmlParser->generateParseTree(tokenList);
		if(htmlTree != 0)
		{
			CHtmlRenderContext context;
			context.dstImg = _dstImg;
			context.imgH = mHeight;
			context.imgW = mWidth;
			CCssDeclaration stl("color:red;");
			context.addStyle(stl);
			context.addText("Hello!");
			context.renderText();
			context.removeTopStyle();
			/*mTree = CHtmlDomNode::createNode(htmlTree);
			CHtmlRenderContext context;
			context.dstImg = _dstImg;
			context.imgH = mHeight;
			context.imgW = mWidth;
			context.font = mDefaultFont;
			//context.addText("Hello from a render context!");
			//context.renderText(0, 0);
			mTree->render(context, 0, 0);
			//renderText(_dstImg, _code, 0, 0);*/
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	/*unsigned CWebPanel::getTextLine(char * _dst, const char * _src, unsigned _maxWidth)
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
	}*/

}	// namespace game
}	// namespace rev
