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
		void RendererBackEnd::setCamera(const math::Mat34f& _view, const math::Mat44f& _proj) {
			Mat34f invView;
			_view.inverse(invView);
			mViewProj = _proj * invView;
		}

		//--------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::render(const RendererBackEnd::StaticGeometry& _geom) {
			mDriver->setShader(_geom.shader);
			unsigned mvpUniform = mDriver->getUniformLocation("mvp");
			Mat44f mvp = mViewProj * _geom.transform;
			mDriver->setUniform(mvpUniform, mvp);

			unsigned worldUniform = mDriver->getUniformLocation("world");
			Mat33f world = _geom.transform;
			mDriver->setUniform(worldUniform, world);

			unsigned lightDirUniform = mDriver->getUniformLocation("lightDir");
			mDriver->setUniform(lightDirUniform, Vec3f::xAxis());
			
			mDriver->setAttribBuffer(0, _geom.nVertices, _geom.vertices);
			mDriver->setAttribBuffer(1, _geom.nVertices, _geom.normals);
			mDriver->drawIndexBuffer(_geom.nIndices, _geom.indices, GraphicsDriver::EPrimitiveType::triangles);
		}

		//--------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::flush() {
			// Intentionally blank
		}
	}
}