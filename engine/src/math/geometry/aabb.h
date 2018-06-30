//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
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

#include <math/algebra/vector.h>
#include <math/algebra/affineTransform.h>
#include <math/linear.h>
#include "ray.h"

namespace rev::math
{
	/// Axis aligned bounding box
	struct AABB {
		using Vector = Vec3f;

		// Constructors
		AABB() = default;
		AABB(const Vector& _min, const Vector& _max)
			: mMin(_min), mMax(_max)
		{ }

		AABB(const Vector& _o, float _size)
			:mMin(_o-_size*0.5f*Vector(1.f,1.f,1.f))
			,mMax(_o+_size*0.5f*Vector(1.f,1.f,1.f))
		{ }

		AABB(const AABB& a, const AABB& b)
			: mMin(math::min(a.min(),b.min()))
			, mMax(math::max(a.max(),b.max()))
		{
		}

		float area() const {
			auto h = mMax-mMin;
			return 2.f*(h.x()*h.y()+h.x()*h.z()+h.y()*h.z());
		}

		AABB transform(const AffineTransform& transform) const
		{
			AABB res;
			res.clear();
			for(int i = 0; i < 8; i++)
			{
				auto x = (i&1) ? mMin.x() : mMax.x();
				auto y = (i&2) ? mMin.y() : mMax.y();
				auto z = (i&4) ? mMin.z() : mMax.z();
				res.add(transform.transformPosition({x,y,z}));
			}
			return res;
		}

		// Size, position and volume
		/// Make the Box empty
		void clear() {
			mMin = Vector(std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity(), std::numeric_limits<float>::infinity());
			mMax = Vector(-std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity(), -std::numeric_limits<float>::infinity());
		}

		bool empty() const {
			return mMax.x() < mMin.x()
				|| mMax.y() < mMin.y()
				|| mMax.z() < mMin.z();
		}

		void add(const Vector& v)
		{
			mMin = math::min(mMin, v);
			mMax = math::max(v, mMax);
		}

		const Vector& min() const { return mMin; }
		const Vector& max() const { return mMax; }
		Vector size() const { return mMax - mMin; }
		Vector origin() const { return 0.5f*(mMin + mMax); }

		bool contains(const Vector& _point) const {
			return (math::min(_point, mMin) == mMin) && (math::max(_point, mMax) == mMax);
		}

		// Intersection and distance
		/// find intersection between this box and a ray, in the ray's parametric interval [_tmin, _tmax]
		/// Also, store the minimun intersection distance into _tout
		bool intersect(const Ray::Implicit& _r, float _tmax, float& _maxEnter) const {
			Vector t1 = (mMin -_r.o).cwiseProduct(_r.n);
			Vector t2 = (mMax -_r.o).cwiseProduct(_r.n);
			// Swapping the order of comparison is important because of NaN behavior
			auto tEnter = math::min(t1,t2);
			auto tLeave = math::max(t2,t1);
			_maxEnter = math::max(tEnter.x(), math::max(tEnter.y(), math::max(tEnter.z(), 0.f)));
			auto minLeave = math::min(tLeave.x(), math::min(tLeave.y(), math::min(tLeave.z(), _tmax)));
			return minLeave >= _maxEnter;
		}
	private:
		Vector mMin;
		Vector mMax;
	};
}