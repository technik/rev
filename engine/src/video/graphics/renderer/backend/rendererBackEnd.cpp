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
		void RendererBackEnd::setView(const math::Mat34f& _view) {
			_view.inverse(mInvView);
		}

		//--------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::setCamera(const math::Mat34f& _view, const math::Mat44f& _proj) {
			_view.inverse(mInvView);
			mViewProj = _proj * mInvView;
		}

		//--------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::render(const RendererBackEnd::StaticGeometry& _geom) {
			//setShader(_geom.shader);
			// Set uniforms
			Mat44f mvp = mViewProj * _geom.transform;
			mDriver->setUniform(mMvpUniform, mvp);
			for(size_t i = 0; i < 3; ++i)
				mDriver->setUniform(mShadowMvpUniform[i], mSvp[i]*_geom.transform);
			Mat33f world = _geom.transform;
			mDriver->setUniform(mWorldRotUniform, world);
			mDriver->setUniform(mColorUniform, _geom.color);
			
			// Set arrays
			mDriver->setAttribBuffer(0, _geom.nVertices, _geom.vertices);
			mDriver->setAttribBuffer(1, _geom.nVertices, _geom.normals);
			mDriver->setAttribBuffer(2, _geom.nVertices, _geom.uvs);

			// Draw
			mDriver->drawIndexBuffer(_geom.nIndices, _geom.indices, 
				_geom.strip?GraphicsDriver::EPrimitiveType::triStrip:GraphicsDriver::EPrimitiveType::triangles);
		}

		//--------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::flush() {
			// Intentionally blank
		}

		//--------------------------------------------------------------------------------------------------------------
		void RendererBackEnd::setShader(Shader::Ptr _newShader) {
			mDriver->setShader((Shader*)_newShader);
			mMvpUniform = mDriver->getUniformLocation("uMvp");
			mShadowMvpUniform[0] = mDriver->getUniformLocation("uShadowMvp0");
			mShadowMvpUniform[1] = mDriver->getUniformLocation("uShadowMvp1");
			mShadowMvpUniform[2] = mDriver->getUniformLocation("uShadowMvp2");
			mWorldRotUniform = mDriver->getUniformLocation("uWorldRot");
			mColorUniform = mDriver->getUniformLocation("uColor");
			mCurShader = _newShader;
		}
	}
}