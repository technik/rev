////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 2nd, 2012
////////////////////////////////////////////////////////////////////////////////
// Fonts

#ifndef _REV_VIDEO_FONT_FONT_H_
#define _REV_VIDEO_FONT_FONT_H_

#include <revCore/string.h>
#include <rtl/map.h>

namespace rev { namespace video
{
	class CStaticModel;
	class CTexture;

	class CFont
	{
	public:
		CFont(const char* _filename);
		~CFont();

		CStaticModel *	createText(const char * _text, unsigned _size);
		CTexture*		getTexture(unsigned _size);

	private:
		class CSizedFont
		{
		public:
			CSizedFont(unsigned _size);
			~CSizedFont();
			CStaticModel *	createText(const char * _text);
			CTexture *		texture();
			
		private:
			class CChar
			{
			public:
			};

			CTexture *				mTexture;
			rtl::map<char, CChar>	mChars;
		};

	private:
		CSizedFont * getFontBySize(unsigned _size);

		rtl::map<unsigned, CSizedFont*>	mFonts;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_FONT_FONT_H_
