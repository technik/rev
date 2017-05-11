//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "camera.h"
#include <video/graphics/renderer/types/renderTarget.h>

namespace rev {
	namespace video {

		using namespace math;

		//--------------------------------------------------------------------------------------------------------------
		Camera::Camera(float _fov, float _near, float _far, bool _ortho, RenderTarget* _dst)
			: mRt(_dst)
			, mFov(_fov)
			, mNear(_near)
			, mFar(_far)
			, mOrtho(_ortho)
		{}

		//--------------------------------------------------------------------------------------------------------------
		Frustum Camera::frustum() const {
			return Frustum(aspectRatio(), mFov, mNear, mFar);
		}

		//--------------------------------------------------------------------------------------------------------------
		Mat44f Camera::projection() const {
			return frustum().projection();
		}

		//--------------------------------------------------------------------------------------------------------------
		Mat34f Camera::view() const {
			if(node()) {
				return node()->transform();
			}
			else
				return Mat34f::identity();
		}

		//--------------------------------------------------------------------------------------------------------------
		float Camera::aspectRatio() const {
			if(mRt)
				return mRt->aspectRatio();
			else // Use default viewport
				return 4.f/3.f;
		}
	}
}