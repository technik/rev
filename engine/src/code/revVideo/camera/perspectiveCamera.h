////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Perspective camera

#ifndef _REV_REVVIDEO_CAMERA_PERSPECTIVECAMERA_H_
#define _REV_REVVIDEO_CAMERA_PERSPECTIVECAMERA_H_

#include "camera.h"

namespace rev { class CVec2; } // Forward declaration

namespace rev { namespace video
{
	class CPerspectiveCamera : public ICamera
	{
	public:
		// ----- Constructor -----
		CPerspectiveCamera(	///< Constructor
			TReal _fov,						///< \param _fov Camera's field of view
			TReal _invRatio,				///< \param invRatio Inverse aspect ratio: Viewport height / viewport width
			const CVec2& _clipDistances);	///< \param clipDistances near and far clipping distances
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_CAMERA_PERSPECTIVECAMERA_H_
