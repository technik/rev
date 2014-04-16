//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Render system's back end, the layer right on top of the video driver
#include "rendererBackEnd.h"

#include <math/algebra/matrix.h>
#include <video/graphics/driver/graphicsDriver.h>

using namespace rev::math;

namespace rev {
	namespace video {
		//--------------------------------------------------------------------------------------------------------------
		RendererBackEnd::RendererBackEnd(GraphicsDriver* _driver)
			:mDriver(_driver)
		{
		}

		//--------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::render(const RendererBackEnd::StaticGeometry& _geom) {
			mDriver->setShader(_geom.shader);
			unsigned mvpUniform = mDriver->getUniformLocation("mvp");
			Mat44f mvp = Mat44f::identity() * _geom.transform;
			mDriver->setUniform(mvpUniform, mvp);
			mDriver->setAttribBuffer(0, _geom.nVertices, _geom.vertices);
			mDriver->drawIndexBuffer(_geom.nIndices, _geom.indices, GraphicsDriver::EPrimitiveType::triangles);
		}

		//--------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::flush() {
			// Intentionally blank
		}
	}
}