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
		void ForwardRenderer::renderContext(const RenderContext& _context) {
			mDriver->setShader((Shader*)mShadowShader);
			mDriver->setRenderTarget(mShadowBuffer);
			for (auto obj : _context) {
				renderObject(*obj);
			}
			// Render pass
			mDriver->setShader((Shader*)mShader);
			mDriver->setRenderTarget(nullptr);
			int uShadowMap = mDriver->getUniformLocation("uShadowMap");
			mDriver->setUniform(uShadowMap, mShadowBuffer->tex);
			mBackEnd->setCamera(_context.camera()->view(), _context.camera()->projection());
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
			geom.shader = mShader;
			geom.transform = _obj.transform();
			if (_obj.mMaterial)
				geom.color = _obj.mMaterial->mDiffuse;
			else
				geom.color = Color(1.f, 1.f, 1.f);
			mBackEnd->render(geom);
		}
	}
}