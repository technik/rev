////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 2nd, 2012
////////////////////////////////////////////////////////////////////////////////
// Fonts

#ifndef _REV_VIDEO_FONT_FONT_H_
#define _REV_VIDEO_FONT_FONT_H_

#include <revCore/resourceManager/managedResource.h>
#include <revCore/string.h>
#include <rtl/map.h>

namespace rev { namespace video
{
	class CStaticModel;
	class CTexture;

	class CFont : public IManagedResource<CFont, const char*, true>
	{
	public:
		CFont(const char* _filename);
		~CFont();

		CTexture * renderText(const char * _text, unsigned _size);

	private:
		class CSizedFont
		{
		public:
			CSizedFont(unsigned _size);
			CTexture * renderText(const char * _text);

		private:
			unsigned mSize;
		};

		string	mFilename;
		rtl::map<unsigned, CSizedFont*>	mFonts;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_FONT_FONT_H_
