////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 30th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Textures

#ifndef _REV_REVVIDEO_TEXTURE_TEXTURE_H_
#define _REV_REVVIDEO_TEXTURE_TEXTURE_H_

#include "revCore/math/vector.h"
#include "revCore/resourceManager/resource.h"
#include "revCore/string.h"

namespace rev { namespace video
{
	class CTexture: public TResource<CTexture, string>
	{		
	public:
		// Constructor and destructor
		CTexture(void * buffer, int width, int height);
		CTexture(const string& _name);
		~CTexture();

		// Accessors
		CVec2		size() const;
		unsigned	id	() const;

	private:
		int			mWidth;
		int			mHeight;
		unsigned	mId;
		void*		mBuffer;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline CVec2 CTexture::size() const
	{
		return CVec2(TReal(mWidth), TReal(mHeight));
	}

	//------------------------------------------------------------------------------------------------------------------
	inline unsigned CTexture::id() const
	{
		return mId;
	}
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_TEXTURE_TEXTURE_H_
