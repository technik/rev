////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 2nd, 2012
////////////////////////////////////////////////////////////////////////////////
// Fonts

#ifndef _REV_VIDEO_FONT_FONT_H_
#define _REV_VIDEO_FONT_FONT_H_

#include <revCore/file/fileBinding.h>
#include <revCore/resourceManager/managedResource.h>
#include <revCore/string.h>

#include <vector.h>

namespace rev { namespace video
{
	class CStaticModel;
	class CTexture;

	class CFont : public IManagedResource<CFont, const char*, true>, public CRecreationFileBinding<CFont>
	{
	public:
		CFont(const char* _filename);
		~CFont();

		CTexture * renderText(const char * _text);

	private:
		void	parseFont		(const char * _text);
		void	parseLine		(const char * _text, unsigned& _cursor);
		void	parseCharLine	(const char * _text, unsigned& _cursor);
		void	skipLine		(const char * _text, unsigned& _cursor);
		int		readNumber		(const char * _text, unsigned& _cursor);
		unsigned textLength		(const char * _text);
		unsigned textHeight		(const char * _text);

	private:
		struct CChar
		{
			CChar()
				:x(0)
				,y(0)
				,width(0)
				,height(0)
				,xoffset(0)
				,yoffset(0)
				,xadvance(0)
			{
			}
			CChar(const CChar& _c)
				:x(_c.x)
				,y(_c.y)
				,width(_c.width)
				,height(_c.height)
				,xoffset(_c.xoffset)
				,yoffset(_c.yoffset)
				,xadvance(_c.xadvance)
			{
			}
			~CChar(){}
			int	x, y, width, height, xoffset, yoffset, xadvance;
		};
		rtl::vector<CChar>	mChars;
		unsigned mImgWidth;
		unsigned char * mImg;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_FONT_FONT_H_
