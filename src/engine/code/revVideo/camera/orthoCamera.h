////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Orthographic camera

#ifndef _REV_REVVIDEO_CAMERA_ORTHOCAMERA_H_
#define _REV_REVVIDEO_CAMERA_ORTHOCAMERA_H_

// Engine headers
#include "camera.h"

#include "revCore/math/matrix.h"
#include "revCore/math/vector.h"

namespace rev { namespace video
{
	// Forward declarations
	class CScene;

	class COrthoCamera: public ICamera
	{
	public:
		COrthoCamera(const CVec2& _apperture, TReal _zMin, TReal _zMax);

		// Accessors
		void			setApperture	(const CVec2& _apperture);
		void			setZRange		(TReal _min, TReal _max);
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_CAMERA_ORTHOCAMERA_H_
