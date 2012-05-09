////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 2nd, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fonts

#include "font.h"

#include <revCore/math/vector.h>
#include <revVideo/scene/model/staticModel.h>
#include <revVideo/texture/texture.h>

namespace rev {
	// Static data
	video::CFont::TManager * video::CFont::sManager = 0;

	namespace video
{

	//------------------------------------------------------------------------------------------------------------------
	CFont::CFont(const char* _filename)
		:mFilename(_filename)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CFont::~CFont()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CTexture * CFont::renderText(const char * _text, unsigned _size)
	{
		rtl::map<unsigned, CSizedFont*>::iterator i = mFonts.find(_size);
		CSizedFont * font = 0;
		if(i == mFonts.end())
		{
			font = new CSizedFont(_size);
			mFonts[_size] = font;
		}
		return font->renderText(_text);
	}

	//------------------------------------------------------------------------------------------------------------------
	CFont::CSizedFont::CSizedFont(unsigned _size)
		:mSize(_size)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CTexture * CFont::CSizedFont::renderText(const char * _text)
	{
		// Unused arguments
		_text;
		unsigned char * buffer = new unsigned char[4 * 256 * 256];
		for(unsigned i = 0; i < 256*256; ++i)
		{
			unsigned j = i >> 8;
			buffer[4*i+0] = char(j ^ (i&255));
			buffer[4*i+1] = char(j ^ (i&255));
			buffer[4*i+2] = char(j ^ (i&255));
			buffer[4*i+3] = 255;
		}
		return new CTexture(buffer, 256, 256);
	}

}	// namespace video
}	// namespace rev
