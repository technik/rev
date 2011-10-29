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

using rtl::multimap;

namespace rev { namespace video
{
	class ICamera;
	//------------------------------------------------------------------------------------------------------------------
	class CViewport
	{
	public:
		typedef multimap<TReal, CViewport*>	TViewportContainer;
	public:
		CViewport(const CVec2& _pos, const CVec2& _size, const TReal layer);
		~CViewport();

		// -- Accessor methods
		const CVec2&	pos			()	const;
		const CVec2&	size		()	const;
		TReal			layer		()	const;
		ICamera *		camera		()	const;
		void			setCamera	(ICamera * _camera);

		static TViewportContainer&	viewports();

	private:
		CVec2		mPos;
		CVec2		mSize;
		TReal		mLayer;
		ICamera*	mCamera;

		static TViewportContainer	sViewports;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline const CVec2& CViewport::pos() const
	{
		return mPos;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline const CVec2& CViewport::size() const
	{
		return mSize;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline TReal CViewport::layer() const
	{
		return mLayer;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline ICamera* CViewport::camera() const
	{
		return mCamera;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CViewport::setCamera(ICamera* _camera)
	{
		mCamera = _camera;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline CViewport::TViewportContainer& CViewport::viewports()
	{
		return sViewports;
	}

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_VIEWPORT_VIEWPORT_H_
