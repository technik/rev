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
			//if (_geom.shader != mCurShader)
				setShader(_geom.shader);
			// Set uniforms
			Mat44f mvp = mViewProj * _geom.transform;
			mDriver->setUniform(mMvpUniform, mvp);
			Mat33f world = _geom.transform;
			mDriver->setUniform(mWorldUniform, world);
			mDriver->setUniform(mLightDirUniform, Vec3f(1.f, 0.2f, 1.5f).normalized());
			mDriver->setUniform(mColorUniform, _geom.color);
			
			// Set arrays
			mDriver->setAttribBuffer(0, _geom.nVertices, _geom.vertices);
			mDriver->setAttribBuffer(1, _geom.nVertices, _geom.normals);
			//mDriver->setAttribBuffer(2, _geom.nVertices, _geom.uv);

			// Draw
			mDriver->drawIndexBuffer(_geom.nIndices, _geom.indices, GraphicsDriver::EPrimitiveType::triangles);
		}

		//--------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::flush() {
			// Intentionally blank
		}

		//--------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::setShader(Shader::Ptr _newShader) {
			mDriver->setShader((Shader*)_newShader);
			mMvpUniform = mDriver->getUniformLocation("uMvp");
			mWorldUniform = mDriver->getUniformLocation("uWorld");
			mLightDirUniform = mDriver->getUniformLocation("uLightDir");
			mColorUniform = mDriver->getUniformLocation("uColor");
			mCurShader = _newShader;
		}
	}
}