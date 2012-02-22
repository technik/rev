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
	class CVideoScene;

	class ICamera: public IComponent
	{
	public:
		// Virtual destructor
		virtual ~ICamera()	{}
		// Camera interface
		CMat34			viewMatrix	()	const;
		const CMat4&	projMatrix	()	const;
		CMat4			viewProj	()	const;	///< Returns view-projection matrix.
		CVideoScene*	scene		()	const;
		void			setScene	(CVideoScene* _scn);
	protected:
		// Protected constructor prevents base class instantiations
		ICamera();
		CMat4			mProjection;
	private:
		CVideoScene*	mScene;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_CAMERA_CAMERA_H_
