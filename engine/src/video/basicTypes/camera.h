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
#include <math/geometry/types.h>

namespace rev {
	namespace video {

		class Camera : public core::Component
		{
		public:
			Camera(const math::Mat44f& _proj = math::Mat44f::identity(), const math::Frustum& _f = math::Frustum())
				: mProj(_proj)
				, mFrustum(_f)
			{}

			const math::Frustum&	frustum() const		{ return mFrustum; }
			math::Mat34f			view() const		{ return node()?node()->transform():math::Mat34f::identity(); }
			const math::Mat44f&		projection() const	{ return mProj; };

		protected:
			void setProjection(const math::Mat44f& _proj) {
				mProj = _proj;
			}

		private:
			math::Mat34f mView;
			math::Mat44f mProj;
			math::Frustum mFrustum;
		};

	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_BASICTYPES_CAMERA_H_