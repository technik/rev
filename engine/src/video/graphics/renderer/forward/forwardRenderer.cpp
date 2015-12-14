//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/15
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer

#include "forwardRenderer.h"
#include "../backend/rendererBackEnd.h"
#include "../renderContext.h"
#include "../renderMesh.h"
#include "../renderObj.h"
#include <video/basicTypes/camera.h>
#include <video/graphics/renderer/material.h>
#include <video/graphics/renderer/types/renderTarget.h>

using namespace rev::math;

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
		void ForwardRenderer::setShadowLight(const math::Vec3f& _pos, const math::Vec3f& dir, const Color& _color) {

		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::renderContext(const RenderContext& _context) {
			// compute light view matrix
			auto camView = _context.camera()->view();
			Mat34f lightView = Mat34f::identity();
			if (abs(mLightDir * camView.col(1)) < 0.5) { // Do not use the view direction if it is too aligned with the light
				auto lightUp = camView.col(1) ^ mLightDir;
				auto lightSide = mLightDir ^ lightUp;
				lightView.setCol(0, lightSide);
				lightView.setCol(1, lightUp);
				lightView.setCol(2, mLightDir);
			}
			else {
				auto lightUp = camView.col(2) ^ mLightDir;
				auto lightSide = mLightDir ^ lightUp;
				lightView.setCol(0, lightSide);
				lightView.setCol(1, lightUp);
				lightView.setCol(2, mLightDir);
			}
			float lightWidth = 100.f;
			float lightHeight = 200.f;

			lightView.setCol(3, mLightPos);//camView.rotate({0.f,0.5f*lightWidth,0.f}));
			Mat44f shadowProj = Mat44f::ortho({lightWidth,lightWidth,lightHeight});

			mBackEnd->setCamera(lightView, shadowProj);

			mDriver->setRenderTarget(mShadowBuffer);
			mDriver->setViewport(math::Vec2i(0,0), mShadowBuffer->tex->size);
			mDriver->clearZBuffer();
			mDriver->setCulling(GraphicsDriver::ECulling::eFront);
			mBackEnd->setShader(mShadowShader);
			for (auto obj : _context) {
				renderObject(*obj);
			}
			//mDriver->finishFrame();
			// Render pass
			mDriver->setRenderTarget(nullptr);
			mDriver->setViewport(math::Vec2i(0,0), mWindowSize);
			mDriver->setClearColor({0.f,0.f,0.f});
			mDriver->clearColorBuffer();
			mDriver->clearZBuffer();
			mDriver->setCulling(GraphicsDriver::ECulling::eBack);
			Mat34f invLight;
			lightView.inverse(invLight);
			mBackEnd->setShadowMvp(shadowProj * invLight);
			mBackEnd->setShader(mShader);
			int uLightDir = mDriver->getUniformLocation("uLightDir");
			mDriver->setUniform(uLightDir, mLightDir);
			int uShadowMap = mDriver->getUniformLocation("uShadowMap");
			mDriver->setUniform(uShadowMap, mShadowBuffer->tex);
			mBackEnd->setCamera(camView, _context.camera()->projection());
			// Render all objects
			for (auto obj : _context) {
				renderObject(*obj);
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::renderObject(const RenderObj& _obj)
		{
			RendererBackEnd::StaticGeometry geom;
			if (_obj.mesh()->stripLength > 0) {
				geom.strip = true;
				geom.indices = _obj.mesh()->triStrip;
				geom.nIndices = _obj.mesh()->stripLength;
			} else {
				geom.strip = false;
				geom.indices = _obj.mesh()->indices;
				geom.nIndices = _obj.mesh()->nIndices;
			}
			geom.vertices = _obj.mesh()->vertices;
			geom.normals = _obj.mesh()->normals;
			geom.nVertices = _obj.mesh()->nVertices;
			geom.transform = _obj.transform();
			if (_obj.mMaterial)
				geom.color = _obj.mMaterial->mDiffuse;
			else
				geom.color = Color(1.f, 1.f, 1.f);
			mBackEnd->render(geom);
		}
	}
}