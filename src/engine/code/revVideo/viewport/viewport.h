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
	class CCamera;
	//------------------------------------------------------------------------------------------------------------------
	class CViewport
	{
	public:
		typedef rtl::multimap<TReal, CViewport*>	TViewportContainer;
	public:
		CViewport(const CVec2& _pos, const CVec2& _size, const TReal layer);
		~CViewport();

		// -- Accessor methods
		const CVec2&	pos			()	const;
		const CVec2&	size		()	const;
		TReal			layer		()	const;
		CCamera *		camera		()	const;
		void			setCamera	(CCamera * _camera);

		static TViewportContainer&	viewports();

	private:
		CVec2		mPos;
		CVec2		mSize;
		TReal		mLayer;
		CCamera*	mCamera;

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
	inline CCamera* CViewport::camera() const
	{
		return mCamera;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CViewport::setCamera(CCamera* _camera)
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
