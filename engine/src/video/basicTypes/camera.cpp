//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "camera.h"
#include <video/graphics/renderer/types/renderTarget.h>
#include <game/scene/transform/objTransform.h>
#include <video/graphics/driver/graphicsDriver.h>

namespace rev {
	namespace video {

		using namespace math;

		//--------------------------------------------------------------------------------------------------------------
		Camera::Camera(float _fov, float _near, float _far, bool _ortho)
			: mFov(_fov)
			, mNear(_near)
			, mFar(_far)
			, mOrtho(_ortho)
		{}

		//--------------------------------------------------------------------------------------------------------------
		Frustum Camera::frustum() const {
			return Frustum(GraphicsDriver::get().nativeFrameBuffer()->aspectRatio(), mFov, mNear, mFar);
		}

		//--------------------------------------------------------------------------------------------------------------
		Mat44f Camera::projection() const {
			return GraphicsDriver::get().projectionMtx(mFov, GraphicsDriver::get().nativeFrameBuffer()->aspectRatio(), mNear, mFar);
		}

		//--------------------------------------------------------------------------------------------------------------
		Mat34f Camera::view() const {
			if(mTransform) {
				return mTransform->matrix();
			}
			else
				return Mat34f::identity();
		}

		//--------------------------------------------------------------------------------------------------------------
		Camera* Camera::construct(const cjson::Json& _data) {
			float fov = (float)_data["fov"] * 3.14159f / 180.f;
			float nearPlane = (float)_data["near"];
			float farPlane = (float)_data["far"];
			return new Camera(fov, nearPlane, farPlane);
		}
	}
}