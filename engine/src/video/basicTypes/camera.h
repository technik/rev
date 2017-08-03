//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// camera (basically a projection and point of view for render)

#ifndef _REV_VIDEO_BASICTYPES_CAMERA_H_
#define _REV_VIDEO_BASICTYPES_CAMERA_H_

#include <math/algebra/matrix.h>
#include <math/geometry/types.h>
#include <cjson/json.h>

namespace rev {
	namespace video {

		class RenderTarget;

		class Camera
		{
		public:
			/// Create a camera that renders into dst target (either a viewport or a render texture)
			/// if dst is nullptr, then the default Viewport will be used as target
			/// near and far ar clipping plane distances
			Camera(float _fov, float _near, float _far, bool ortho = false, RenderTarget* dst = nullptr);

			// Public accessors
			void setRenderTarget(RenderTarget* _rt)		{ mRt = _rt; }
			void setFov(float _fov)						{ mFov = _fov; }
			void setClipPlanes(float _near, float _far)	{ mNear = _near; mFar = _far; }
			math::Mat34f view() const;

			float fov		() const					{ return mFov; }
			float nearPlane	() const					{ return mNear; }
			float farPlane	() const					{ return mFar; }

			math::Frustum	frustum		() const;
			math::Mat44f	projection	() const;

			static Camera* construct(const cjson::Json&);

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