//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "camera.h"
#include <video/graphics/renderer/types/renderTarget.h>

namespace rev {
	namespace video {

		using namespace math;

		//--------------------------------------------------------------------------------------------------------------
		Camera::Camera(float fov, float near, float far, bool ortho, RenderTarget* dst)
			: mRt(dst)
			, mFov(fov)
			, mNear(near)
			, mFar(far)
			, mOrtho(ortho)
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
			if(node())
				return node()->transform();
			else
				return Mat34f::identity();
		}

		//--------------------------------------------------------------------------------------------------------------
		float Camera::aspectRatio() const {
			if(mRt)
				return mRt->aspectRatio();
			else // Use default viewport
				return 3.f/4.f;
		}
	}
}