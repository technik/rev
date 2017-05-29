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
			for (auto cam : mCameras)
				mRenderer.render(mObjects, *cam);
			return true;
		}

		//--------------------------------------------------------------------------------------------------------------
		RenderObj* RenderLayer::createRenderObj(const Json& _data) {
			RenderObj* obj = RenderObj::construct(_data);
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