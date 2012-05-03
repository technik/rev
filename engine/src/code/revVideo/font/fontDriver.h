////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 3rd, 2012
////////////////////////////////////////////////////////////////////////////////
// Font driver. Deals with FreeType library

#ifndef _REV_VIDEO_FONT_FONTDRIVER_H_
#define _REV_VIDEO_FONT_FONTDRIVER_H_

// Free type headers
#include <ft2build.h>
#include FT_FREETYPE_H

#include <revCore/string.h>
#include <rtl/map.h>

namespace rev { namespace video
{
	class SFontDriver
	{
	public:
		static SFontDriver*	get();
		void				release();

		void				loadFont(const char * _filename);

	private:
		SFontDriver();
		~SFontDriver();

		static unsigned nRefs;
		static SFontDriver* sInstance;

	private:
		FT_Library					mLibrary;
		FT_Face						mCurFont;
		rtl::map<string, FT_Face>	mFontFaces;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_FONT_FONTDRIVER_H_
