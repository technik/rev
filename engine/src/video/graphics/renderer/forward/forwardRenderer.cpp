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
			mDebug->drawBasis(Mat34f::identity());
			mDebug->render();
			//mDriver->finishFrame();
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::setShadowLight(const math::Vec3f& _pos, const math::Vec3f& dir, const Color& _color) {

		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::renderContext(const RenderContext& _context) {
			auto camView = _context.camera()->view();
			auto pov = (mDebugCamera?mDebugCamera:_context.camera())->view();
			mDebug->drawLine(camView.col(3), camView*Vec3f(0.f,100.f,0.f), Color(1.f,0.f,1.f));
			if (mDebugCamera) {
				mShadowPass->mDebug = mDebug;
				mDebug->setViewProj(pov, mDebugCamera->projection());
				mDebug->drawBasis(camView);
				mDebug->drawFrustum(camView, _context.camera()->frustum(), Color(1.f,0.f,1.f));
			}
			else {
				mShadowPass->mDebug = nullptr;
				mDebug->setViewProj(pov, _context.camera()->projection());
			}
			// Render shadow pass
			Frustum globalFrustum = _context.camera()->frustum();
			mShadowPass->config(mLightDir, camView, globalFrustum);
			for (auto obj : _context) {
				renderObject(*obj);
			}
			//mDriver->finishFrame();
			// Render pass
			//mDriver->setMultiSampling(false);
			mDriver->setRenderTarget(nullptr);
			mDriver->setViewport(math::Vec2i(0,0), mWindowSize);
			mDriver->setClearColor(Color(0.7f, 0.8f, 1.f, 1.f));
			mDriver->clearColorBuffer();
			mDriver->clearZBuffer();
			mDriver->setCulling(GraphicsDriver::ECulling::eBack);
			mBackEnd->setShadowMvp(mShadowPass->viewProj());
			mBackEnd->setShader(mShader);
			int uLightDir = mDriver->getUniformLocation("uLightDir");
			mDriver->setUniform(uLightDir, mLightDir);
			int uShadowMap = mDriver->getUniformLocation("uShadowMap");
			mDriver->setUniform(uShadowMap, mShadowPass->tex());
			//int uFarShadowMap = mDriver->getUniformLocation("uFarShadowMap");
			//mDriver->setUniform(uFarShadowMap, mFarShadowPass->tex());
			if(mDebugCamera)
				mBackEnd->setCamera(pov, mDebugCamera->projection());
			else
				mBackEnd->setCamera(pov, _context.camera()->projection());
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