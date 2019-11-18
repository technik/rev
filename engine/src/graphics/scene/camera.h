//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once

#include <math/algebra/matrix.h>
#include <math/algebra/affineTransform.h>
#include <math/geometry/aabb.h>
#include <math/geometry/types.h>
#include <cassert>

#ifdef _WIN32
// Undef win32 crap
#undef near
#undef far
#endif // _WIN32

namespace rev::gfx {

	class Camera {
	public:
		Camera(float fov = math::Pi/3, float near = 0.1f, float far = 10000)
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
			return projection(_aspectRatio) * mView;
		}

		const math::Mat44f& view() const { return mView; }

		math::Mat44f projection(float _aspectRatio) const
		{
			assert(mNear != 0 && mFar > mNear);
			//auto projectionMatrix = math::orthographicMatrix({4.f, 2.f}, mNear, mFar);//(mFov, _aspectRatio, mNear, mFar);
			return math::frustumMatrix(mFov, _aspectRatio, mNear, mFar);
		}

		void setWorldTransform(const math::AffineTransform& _x)
		{
			mWorld = _x;
			mView = _x.orthoNormalInverse().matrix();
		}

		// aabb in the local frame of the camera
		math::AABB aabb(float near, float far, float aspectRatio) const;

		// World space accessors
		math::Vec3f position() const { return mWorld.position(); }
		math::Vec3f viewDir() const { return mWorld.rotateDirection(math::Vec3f(0.f,1.f,0.f)); }
		const math::AffineTransform& world() const { return mWorld; }

		// Camera parameters
		auto near() const { return mNear; }
		auto far() const { return mFar; }
		auto fov() const { return mFov; }
		math::Frustum frustum(float aspectRatio) const { return math::Frustum(aspectRatio, mFov, mNear, mFar); }

	private:
		float mFov, mNear, mFar;
		math::AffineTransform mWorld;
		math::Mat44f mView;
	};

	// Inline implementation
	inline math::AABB Camera::aabb(float _near, float _far, float _aspectRatio) const {
		auto zMax = _far * std::tan(mFov/2);
		auto xMax = _aspectRatio * zMax;
		auto minP = math::Vec3f(-xMax, _near, -zMax);
		auto maxP = math::Vec3f(xMax, _far, zMax);
		return math::AABB(minP, maxP);
	}

}
