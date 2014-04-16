//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/15
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer

#include "forwardRenderer.h"
#include "../backend/rendererBackEnd.h"
#include "../renderObj.h"

using namespace rev::game;

namespace rev {
	namespace video {
		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::startFrame()
		{
			mDriver->clearColorBuffer();
			mDriver->clearZBuffer();
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::finishFrame()
		{
			mDriver->finishFrame();
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::setCamera(const Camera& _cam) {
			mBackEnd->setCamera(_cam.view(), _cam.projection());
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::renderObject(const RenderObj& _obj)
		{
			RendererBackEnd::StaticGeometry geom;
			geom.indices = _obj.indices;
			geom.nIndices = _obj.nIndices;
			geom.vertices = _obj.vertices;
			geom.nVertices = _obj.nVertices;
			geom.shader = mShader;
			geom.transform = _obj.transform;
			mBackEnd->render(geom);
		}
	}
}