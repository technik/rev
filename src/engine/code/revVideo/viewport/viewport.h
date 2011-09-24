////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video driver interface
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// Viewports

#ifndef _REV_REVVIDEO_VIEWPORT_VIEWPORT_H_
#define _REV_REVVIDEO_VIEWPORT_VIEWPORT_H_

// Engine headers
#include "revCore/math/vector.h"
#include "rtl/multimap.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	class CViewport
	{
	public:
		CViewport(const CVec2& _pos, const CVec2& _size, const TReal layer);
		~CViewport();

		// -- Accessor methods
		const CVec2&	pos		()	const;
		const CVec2&	size	()	const;
		TReal			mLayer	()	const;

	private:
		CVec2	mPos;
		CVec2	mSize;
		TReal	mLayer;

		static rtl::multimap<TReal, CViewport*>	sViewports;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_VIEWPORT_VIEWPORT_H_
