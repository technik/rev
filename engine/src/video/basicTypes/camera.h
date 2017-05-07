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

		class RenderTarget;

		class Camera : public core::Component
		{
		public:
			/// Create a camera that renders into dst target (either a viewport or a render texture)
			/// if dst is nullptr, then the default Viewport will be used as target
			/// near and far ar clipping plane distances
			Camera(float fov, bool ortho, float near, float far, RenderTarget* dst);

			// Public accessors
			void setRenderTarget(RenderTarget* _rt)		{ mRt = _rt; }
			void setFov(float _fov)						{ mFov = _fov; }
			void setClipPlanes(float near, float far)	{ mNear = near; mFar = far;}

			float fov() const							{ return mFov; }
			float near() const							{ return mNear; }
			float far() const							{ return mFar; }

			math::Frustum	frustum		() const;
			math::Mat44f	projection	() const;

		private:
			float aspectRatio() const;

			RenderTarget*	mRt;
			float	mFov;
			float	mNear, mFar;
			bool	mOrtho;
		};

	}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_BASICTYPES_CAMERA_H_