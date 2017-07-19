//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "renderLayer.h"
#include <video/basicTypes/camera.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderObj.h>

using namespace cjson;

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		bool RenderLayer::update(float _dt) {
#ifdef OPENGL_45
			for (auto cam : mCameras)
				mRenderer.render(mObjects, *cam);
#endif // OPENGL_45
			return true;
		}

		//--------------------------------------------------------------------------------------------------------------
		RenderObj* RenderLayer::createRenderObj(const Json& _data) {
#ifdef OPENGL_45
			RenderObj* obj = RenderObj::construct(_data);
#else
			RenderObj* obj = nullptr;
			assert(false);
#endif
			if(obj)
				mObjects.push_back(obj);
			return obj;
		}

		//--------------------------------------------------------------------------------------------------------------
		Camera* RenderLayer::createCamera(const Json& _data) {
			Camera* cam = Camera::construct(_data);
			if (cam)
				mCameras.push_back(cam);
			return cam;
		}

} }	// namespace rev::video