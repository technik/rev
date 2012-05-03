////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 3rd, 2012
////////////////////////////////////////////////////////////////////////////////
// Font driver. Deals with FreeType library

#include "fontDriver.h"
#include <revCore/codeTools/assert/assert.h>
#include <revCore/file/file.h>

using namespace rev::codeTools;

namespace rev { namespace video
{
	SFontDriver * SFontDriver::sInstance = 0;
	unsigned SFontDriver::nRefs = 0;

	//------------------------------------------------------------------------------------------------------------------
	SFontDriver * SFontDriver::get()
	{
		if(0 == nRefs)
			sInstance = new SFontDriver();
		nRefs++;
		return sInstance;
	}

	//------------------------------------------------------------------------------------------------------------------
	void SFontDriver::release()
	{
		nRefs--;
		if(!nRefs)
			delete sInstance;
	}

	//------------------------------------------------------------------------------------------------------------------
	void SFontDriver::loadFont(const char * _fontName)
	{
		FT_Face fontFace;
		if(mFontFaces.find(string(_fontName)) == mFontFaces.end())
		{
			CFile file(_fontName);
			int error = FT_New_Memory_Face(	mLibrary, 
											reinterpret_cast<const FT_Byte*>(file.buffer()),
											file.size(),
											0,
											&fontFace);
			if(error)
			{
				rev::revLog("Error ");
				rev::revLog(error);
				rev::revLog(" loading font ");
				revAssert(false, _fontName);
			}
			mFontFaces[string(_fontName)] = fontFace;
		}
		else
		{
			fontFace = mFontFaces[string(_fontName)];
		}
		mCurFont = fontFace;
	}

	//------------------------------------------------------------------------------------------------------------------
	SFontDriver::SFontDriver()
	{
		int error = FT_Init_FreeType(&mLibrary);
		if(error)
		{
			rev::revLog("Error ");
			rev::revLog(error);
			revAssert(false, " initializing Freetype");
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	SFontDriver::~SFontDriver()
	{
	}
	
}	// namespace video
}	// namespace rev
