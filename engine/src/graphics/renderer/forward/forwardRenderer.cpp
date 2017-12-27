//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/15
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer

#include "forwardRenderer.h"
#include <core/components/affineTransform.h>
#include "../backend/rendererBackEnd.h"
#include <video/basicTypes/camera.h>
#include <video/graphics/renderScene.h>
#include <video/graphics/renderObj.h>
#include <video/graphics/staticRenderMesh.h>
#include <video/graphics/material.h>
//#include "../renderMesh.h"
//#include "../../renderObj.h"
//#include <video/graphics/renderer/types/renderTarget.h>

using namespace rev::math;

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::init() {
			glClearColor(0.f,1.f,0.f,1.f);
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::render() {
			glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		}
	}
}