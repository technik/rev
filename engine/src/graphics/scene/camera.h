//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "math/algebra/matrix.h"
#include "math/algebra/affineTransform.h"

namespace rev { namespace graphics {

	class Camera {
	public:
		Camera(float fov, float near, float far)
			: mFov(fov)
			, mNear(near)
			, mFar(far)
		{
			mView = math::Mat44f::identity();
		}

		math::Mat44f viewProj(float _aspectRatio) const
		{
			auto projectionMatrix = math::frustrumMatrix(mFov, _aspectRatio, mNear, mFar);
			return projectionMatrix * mView;
		}

		void setWorldMatrix(const math::AffineTransform& _x)
		{
			mView.block<3,4>(0,0) = _x.inverse().matrix();
		}

	private:
		float mFov, mNear, mFar;

		math::Mat44f mView;
	};

}}
