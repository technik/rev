//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/10
//----------------------------------------------------------------------------------------------------------------------
// camera (basically a point of view for render)

#ifndef _REV_GRAPHIS3D_CAMERA_CAMERA_H_
#define _REV_GRAPHIS3D_CAMERA_CAMERA_H_

#include <revMath/algebra/matrix.h>

namespace rev { namespace graphics3d {

	class Camera
	{
	public:
		Camera(const math::Mat44f& _proj = math::Mat44f::identity()
			, const math::Mat34f& _view = math::Mat34f::identity());

		void			setProjection	(const math::Mat44f& _proj);
		void			setView			(const math::Mat34f& _view);
		math::Mat44f	viewProjMatrix	() const;

	private:
		math::Mat34f mInvView;
		math::Mat44f mProj;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementation
	//------------------------------------------------------------------------------------------------------------------
	inline Camera::Camera(const math::Mat44f& _proj, const math::Mat34f& _view)
		:mProj(_proj)
	{
		_view.inverse(mInvView);
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void Camera::setProjection	(const math::Mat44f& _proj) { mProj = _proj; }
	inline void Camera::setView			(const math::Mat34f& _view)	{ _view.inverse(mInvView); }

	//------------------------------------------------------------------------------------------------------------------
	inline math::Mat44f Camera::viewProjMatrix() const { return mProj * mInvView; }

}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GRAPHIS3D_RENDERER_RENDERER_H_