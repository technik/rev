//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// camera (basically a projection and point of view for render)

#ifndef _REV_VIDEO_BASICTYPES_CAMERA_H_
#define _REV_VIDEO_BASICTYPES_CAMERA_H_

#include <math/algebra/matrix.h>
#include <core/components/component.h>
#include <core/components/sceneNode.h>

namespace rev {
	namespace video {

		class Camera : public core::Component
		{
		public:
			Camera(const math::Mat44f& _proj = math::Mat44f::identity(), const math::Vec3f& bbMin = {-1.f,-1.f,-1.f}, const math::Vec3f& bbMax = { 1.f,1.f,1.f })
				: mProj(_proj)
				, mBBMin(bbMin)
				, mBBMax(bbMax)
			{}

			math::Mat34f		view() const		{ return node()?node()->transform():math::Mat34f::identity(); }
			const math::Mat44f& projection() const	{ return mProj; };

		protected:
			void setProjection(const math::Mat44f& _proj) {
				mProj = _proj;
			}

		private:
			math::Mat34f mView;
			math::Mat44f mProj;
			math::Vec3f mBBMin;
			math::Vec3f mBBMax;
		};

	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_BASICTYPES_CAMERA_H_