//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "math/algebra/matrix.h"
#include "math/algebra/affineTransform.h"
#include <cassert>

#ifdef _WIN32
// Undef win32 crap
#undef near
#undef far
#endif // _WIN32

namespace rev { namespace graphics {

	class Camera {
	public:
		Camera(float fov = 0.8f, float near = 0.1f, float far = 10000)
			: mFov(fov)
			, mNear(near)
			, mFar(far)
		{
			assert(mNear != 0 && mFar > mNear);
			mView = math::Mat44f::identity();
			mWorld = math::AffineTransform::identity();
		}

		math::Mat44f viewProj(float _aspectRatio) const
		{
			assert(mNear != 0 && mFar > mNear);
			auto projectionMatrix = math::frustrumMatrix(mFov, _aspectRatio, mNear, mFar);
			return projectionMatrix * mView;
		}

		void setWorldTransform(const math::AffineTransform& _x)
		{
			mWorld = _x;
			mView.block<3,4>(0,0) = _x.inverse().matrix();
		}

		const math::Vec3f& position() const { return mWorld.position(); }

	private:
		float mFov, mNear, mFar;

		math::AffineTransform mWorld;
		math::Mat44f mView;
	};

}}
