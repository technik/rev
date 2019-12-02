//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#ifndef _REV_MATH_GEOMETRY_TYPES_H_
#define _REV_MATH_GEOMETRY_TYPES_H_

#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>

namespace rev {
	namespace math {
		
		struct Plane
		{
			Vec3f normal;
			float t;
		};

		inline Plane operator*(const Mat44f& m, const Plane& p)
		{
			Vec4f origin = Vec4f(p.t * p.normal, 1.f);
			Vec4f dir = Vec4f(p.normal, 0.f);
			Vec4f transformedDir = m * dir;
			Vec4f transformedOrigin = m * origin;
			Plane result;
			result.normal = transformedDir.xyz();
			result.t = dot(transformedDir, transformedOrigin);
			return result;
		}

		struct Frustum {
			Frustum() = default; // Allow empty construction
			Frustum(float _aspectRatio, float _yFov, float _near, float _far)
				:mAspectRatio(_aspectRatio)
				,mYFov(_yFov)
				,mNear(_near)
				,mFar(_far)
			{
				mProjection = frustumMatrix(_yFov, _aspectRatio, _near, _far);

				float yTangent = tan(_yFov / 2.f);
				float xTangent = yTangent * _aspectRatio;
				// Normals point outwards
				mPlanes[0] = { Vec3f(0.f,0.f,1.f), -_near }; // Near plane
				mPlanes[1] = { Vec3f(0.f,0.f,-1.f), _far }; // Far plane
				Vec3f topEdgeDir = normalize(Vec3f(0.f, yTangent, -1.f));
				mPlanes[2] = { cross(Vec3f(1.f,0.f,0.f), topEdgeDir), 0.f }; // Top plane
				Vec3f bottomEdgeDir = normalize(Vec3f(0.f, -yTangent, -1.f));
				mPlanes[3] = { cross(Vec3f(-1.f,0.f,0.f), bottomEdgeDir), 0.f }; // Bottom plane
				Vec3f rightEdgeDir = normalize(Vec3f(xTangent, 0.f, -1.f));
				mPlanes[4] = { cross(Vec3f(0.f,-1.f,0.f), rightEdgeDir), 0.f }; // Right plane
				Vec3f leftEdgeDir = normalize(Vec3f(-xTangent, 0.f, -1.f));
				mPlanes[5] = { cross(Vec3f(0.f,1.f,0.f), leftEdgeDir), 0.f }; // Left plane
			}

			float aspectRatio	() const { return mAspectRatio; }
			float fov			() const { return mYFov; }
			float nearPlane		() const { return mNear; }
			float farPlane		() const { return mFar; }
			float centroid		() const { return 0.5f*(mNear+mFar); }
			const Plane& plane(size_t i) const { return mPlanes[i]; }

			const Mat44f& projection() const { return mProjection; }

		private:
			float mAspectRatio;
			float mYFov;
			float mNear;
			float mFar;
			Mat44f mProjection;
			Plane mPlanes[6];

			friend inline Frustum operator*(const Mat44f& m, const Frustum& f);
		};

		inline Frustum operator*(const Mat44f& m, const Frustum& f)
		{
			Frustum result = f;
			for (size_t i = 0; i < 6; ++i)
			{
				result.mPlanes[i] = m * f.plane(i);
			}
			return result;
		}

		inline bool cull(const Frustum& frustum, const Mat44f& worldFromFrustum, const AABB& aabb)
		{
			for (size_t i = 0; i < 6; ++i)
			{
				Plane rotatedPlane = worldFromFrustum * frustum.plane(i);
				Vec3f v1 = aabb.min().cwiseProduct(rotatedPlane.normal);
				Vec3f v2 = aabb.max().cwiseProduct(rotatedPlane.normal);
				Vec3f vMin = math::min(v1, v2);
				float tMin = vMin.x()+ vMin.y()+ vMin.z();
				if (tMin > rotatedPlane.t) // Fully outside
					return false;
			}
			return true;
		}

		inline bool cull(const Frustum& frustum, const AABB& aabb)
		{
			for (size_t i = 0; i < 6; ++i)
			{
				auto& plane = frustum.plane(i);
				Vec3f v1 = aabb.min().cwiseProduct(plane.normal);
				Vec3f v2 = aabb.max().cwiseProduct(plane.normal);
				Vec3f vMin = math::min(v1, v2);
				float tMin = vMin.x() + vMin.y() + vMin.z();
				if (tMin > plane.t) // Fully outside
					return false;
			}
			return true;
		}

} }	// namespace rev::math

#endif // _REV_MATH_GEOMETRY_TYPES_H_