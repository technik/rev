////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// camera interface

#ifndef _REV_REVVIDEO_CAMERA_CAMERA_H_
#define _REV_REVVIDEO_CAMERA_CAMERA_H_

#include "revCore/component/component.h"
#include "revCore/math/matrix.h"

namespace rev { namespace video
{
	// Forward declarations
	class CScene;

	class ICamera: public IComponent
	{
	public:
		// Virtual destructor
		ICamera();
		virtual ~ICamera()	{}
		// Camera interface
		const CMat4&	projMatrix	()	const;
		CMat4			viewProj	()	const;	///< Returns view-projection matrix.
		const CScene*	scene		()	const;
		void			setScene	(const CScene* _scn);
	private:
		CMat4			mProjection;
		const CScene*	mScene;
	};

	//------------------------------------------------------------------------------------------------------------------
	const CMat4& ICamera::projMatrix() const
	{
		return mProjection;
	}

	//------------------------------------------------------------------------------------------------------------------
	const CScene* ICamera::scene() const
	{
		return mScene;
	}

	//------------------------------------------------------------------------------------------------------------------
	void ICamera::setScene(const CScene* _scn)
	{
		mScene = _scn;
	}

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_CAMERA_CAMERA_H_
