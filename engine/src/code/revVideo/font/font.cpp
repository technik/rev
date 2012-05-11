////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 2nd, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fonts

#include "font.h"

#include <revCore/file/file.h>
#include <revCore/math/vector.h>
#include <revCore/string.h>
#include <revVideo/scene/model/staticModel.h>
#include <revVideo/texture/texture.h>

namespace rev {
	// Static data
	video::CFont::TManager * video::CFont::sManager = 0;

	namespace video
{

	//------------------------------------------------------------------------------------------------------------------
	CFont::CFont(const char* _filename)
		:CRecreationFileBinding(_filename)
	{
		mChars.resize(256); // Pre allocate 256 characters
		// Load the font image to a buffer
		char imageName[256];
		unsigned nameLen = stringLength(_filename);
		copyStringN(imageName, _filename, nameLen);
		imageName[nameLen-3] = 'p';
		imageName[nameLen-2] = 'n';
		imageName[nameLen-1] = 'g';

		CTexture fontImage(imageName);
		unsigned width = (unsigned)fontImage.width();
		unsigned height = (unsigned)fontImage.height();
		mImgWidth = width;

		// Copy the image into a buffer
		unsigned imgSize = 4*width*height;
		mImg = new unsigned char[imgSize];
		for(unsigned j = 0; j < height; ++j)
			for(unsigned i = 0; i < width; ++i)
			{
				mImg[4*(i+width*j)+0] = fontImage.buffer()[4*(i+width*(height-j-1))+0];
				mImg[4*(i+width*j)+1] = fontImage.buffer()[4*(i+width*(height-j-1))+1];
				mImg[4*(i+width*j)+2] = fontImage.buffer()[4*(i+width*(height-j-1))+2];
				mImg[4*(i+width*j)+3] = fontImage.buffer()[4*(i+width*(height-j-1))+3];
			}
		//for(unsigned i = 0; i < imgSize; ++i)
		//	mImg[i] = fontImage.buffer()[i];

		// Open the font file
		CFile	file(_filename);
		parseFont(file.textBuffer());
	}

	//------------------------------------------------------------------------------------------------------------------
	CFont::~CFont()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CTexture * CFont::renderText(const char * _text)
	{
		unsigned len = textLength(_text);
		unsigned h = textHeight(_text);
		unsigned bufferSize = 4 * len * h;
		unsigned char * buffer = new unsigned char[bufferSize];
		for(unsigned n = 0; n < bufferSize; ++n)
			buffer[n] = 0;
		unsigned nChars = stringLength(_text);
		unsigned xPos = 0;
		//unsigned yPos = 0;
		for(unsigned c = 0; c < nChars; ++c)
		{
			if(_text[c] == '\n')
			{
				xPos = 0;
				continue;
			}
			CChar cChar = mChars[_text[c]];
			for(int i = 0; i < cChar.width; ++i)
			{
				for(int j = 0; j < cChar.height; ++j)
				{
					unsigned idx = unsigned(xPos+i+len*j);
					unsigned texel = unsigned(cChar.x + i + (cChar.y+cChar.height-j-1)*mImgWidth);
					revAssert(4*idx+3 < bufferSize);
					buffer[4*idx+0] = 255;
					buffer[4*idx+1] = 255;
					buffer[4*idx+2] = 255;
					buffer[4*idx+3] = mImg[4*texel+0];
				}
			}
			xPos += cChar.xadvance;
		}
		return new CTexture(buffer, len, h);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CFont::parseFont(const char * _font)
	{
		unsigned cursor = 0;
		while(_font[cursor] != '\0')
		{
			parseLine(_font, cursor);
		}
	};

	//------------------------------------------------------------------------------------------------------------------
	void CFont::parseLine(const char * _font, unsigned& _cursor)
	{
		if(compareString(&_font[_cursor], "char id=", 8))
		{
			parseCharLine(_font, _cursor);
		}
		else
		{
			skipLine(_font, _cursor);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	int CFont::readNumber(const char * _font, unsigned & _cursor)
	{
		while(_font[_cursor] != '=')
			++_cursor; // Skip everything but numbers
		++_cursor;
		
		char buffer[16];
		unsigned bIdx = 0;
		while((_font[_cursor] != ' ') && (_font[_cursor] != '\t')) // Copy the number to a buffer
		{
			buffer[bIdx++] = _font[_cursor++];
		}
		buffer[bIdx] = '\0';

		return integerFromString(buffer);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CFont::skipLine(const char * _font, unsigned & _cursor)
	{
		while((_font[_cursor] != '\n') && (_font[_cursor] != '\br'))
			++_cursor;
		++_cursor;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CFont::parseCharLine(const char * _font, unsigned & _cursor)
	{
		unsigned char id = (unsigned char)readNumber(_font, _cursor);
		mChars[id].x = readNumber(_font, _cursor);
		mChars[id].y = readNumber(_font, _cursor);
		mChars[id].width = readNumber(_font, _cursor);
		mChars[id].height = readNumber(_font, _cursor);
		mChars[id].xoffset = readNumber(_font, _cursor);
		mChars[id].yoffset = readNumber(_font, _cursor);
		mChars[id].xadvance = readNumber(_font, _cursor);
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CFont::textLength(const char* _text)
	{
		unsigned l = 0;
		for(unsigned i = 0; _text[i] != '\0'; ++i)
		{
			l += mChars[_text[i]].xadvance;
		}
		return l;
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned CFont::textHeight(const char* _text)
	{
		unsigned h = 0;
		for(unsigned i = 0; _text[i] != '\0'; ++i)
		{
			unsigned h1 = mChars[_text[i]].height;
			h=h1>h?h1:h;
		}
		return h;
	}

}	// namespace video
}	// namespace rev
