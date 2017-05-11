//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/15
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer

#include "forwardRenderer.h"
#include "../backend/rendererBackEnd.h"
#include <video/basicTypes/camera.h>
#include <video/graphics/renderScene.h>
#include <video/graphics/renderObj.h>
#include <video/graphics/staticRenderMesh.h>
//#include "../renderMesh.h"
//#include "../../renderObj.h"
//#include <video/graphics/renderer/material.h>
//#include <video/graphics/renderer/types/renderTarget.h>

using namespace rev::math;

namespace rev {
	namespace video {

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::init(GraphicsDriver* _driver) {
			mDriver = _driver;
			mProgram = Shader::manager()->get("shader");
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::render(const RenderScene& _scene, const Camera& _cam, const RenderTarget* _rt) const {
			// TODO: ShadowPass?
			// TODO: Sort into render queues based on material
			// Set global uniforms
			// mDriver->setRenderTarget(_rt);
			mDriver->setViewport(Vec2i(0, 0), Vec2u(800,600));
			mDriver->clearZBuffer();
			mDriver->clearColorBuffer();
			mDriver->setShader(mProgram);
			/*int uWorldViewProj = mDriver->getUniformLocation("uWorldViewProj");
			if(uWorldViewProj == -1)
				return;
			// Camera 
			Mat34f invView;
			_cam.view().inverse(invView);
			Mat44f viewProj = _cam.projection() * invView;
			// render objects
			Mat44f worldViewProj;
			for (const auto obj : _scene.objects) {
				worldViewProj = viewProj * obj->transform();
				mDriver->setUniform(uWorldViewProj, worldViewProj);
				StaticRenderMesh* mesh = obj->mesh();
				// TODO
				glBindBuffer(GL_ARRAY_BUFFER, mesh->vbo);
				glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, 3*sizeof(float), (GLvoid*)0);
				glEnableVertexAttribArray(0);
				glDrawElements(GL_TRIANGLES, mesh->nIndices, GL_SHORT, mesh->indices);
			}*/
		}

		//--------------------------------------------------------------------------------------------------------------
		/*void ForwardRenderer::init(GraphicsDriver* _driver) {
			mDriver = _driver;
			mBackEnd = new RendererBackEnd(mDriver);
			mShader = Shader::manager()->get("forward");
			//mSkyShader = Shader::manager()->get("skybox");
			mDebug = new DebugDrawer(mDriver, mBackEnd);

			mShadowPass[0] = new ShadowPass(mDriver, mBackEnd, 1024);
			mShadowPass[1] = new ShadowPass(mDriver, mBackEnd, 1024);
			mShadowPass[2] = new ShadowPass(mDriver, mBackEnd, 1024);

			mDriver->setShader(mShader);
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::end() {
			delete mShadowPass[0];
			delete mShadowPass[1];
			delete mShadowPass[2];
			delete mBackEnd;
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::startFrame()
		{
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
			// Render shadow pass
			/*Frustum globalFrustum = adjustShadowFrustum(_context);
			float maxFar = globalFrustum.farPlane();
			float farPlane[3] = {maxFar*0.1f, maxFar * 0.3f, maxFar };
			for(size_t i = 0; i < 3; ++i) {
				Frustum cascadeFrustum = Frustum(globalFrustum.aspectRatio(), globalFrustum.fov(), globalFrustum.nearPlane(), farPlane[i]);
				mShadowPass[i]->config(_context.lightDir(), camView, cascadeFrustum, maxFar);
				
				for (auto obj : _context) {
					if(obj->castShadows)
						renderObject(*obj);
				}
				mShadowPass[i]->finishPass();
				//mShadowPass[i]->tex()->generateMipMaps();
			}
			//mDriver->finishFrame();
			if (mDebugCamera) {
				for (size_t i = 0; i < 3; ++i)
					mShadowPass[i]->mDebug = mDebug;
				mDebug->drawLine(camView.col(3), camView*Vec3f(0.f,globalFrustum.farPlane(),0.f), Color(1.f,0.f,1.f));
				mDebug->setViewProj(pov, mDebugCamera->projection());
				mDebug->drawBasis(camView);
				mDebug->drawFrustum(camView, globalFrustum, Color(1.f,0.f,1.f));
			}
			else {
				for (size_t i = 0; i < 3; ++i)
					mShadowPass[i]->mDebug = nullptr;
				mDebug->setViewProj(pov, _context.camera()->projection());
			}*/
			// Render pass
			//mDriver->setMultiSampling(false);
			//mDriver->setRenderTarget(nullptr);
			//mDriver->setViewport(math::Vec2i(0, 0), mWindowSize);
			// Render skybox
/*			if (_context.skyBox) {
				mDriver->setZCompare(false); // Use the skybox to clear the buffer
				mDriver->setShader(mSkyShader);
				int uMap = mDriver->getUniformLocation("uSkyMap");
//				mDriver->setUniform(uMap, _context.skyBox->mMaterial->mDiffMap);
				Mat34f centeredView = pov;
				centeredView.setCol(3, Vec3f::zero());
				if (mDebugCamera)
					mBackEnd->setCamera(centeredView, mDebugCamera->projection());
				else
					mBackEnd->setCamera(centeredView, _context.camera()->projection());
				renderObject(*_context.skyBox);
				mDriver->setZCompare(true);
			} else {*/
				//mDriver->setClearColor(Color(0.7f, 0.8f, 1.f, 1.f));
				//mDriver->clearColorBuffer();
			//}
			/*mDriver->clearZBuffer();
			mDriver->setCulling(GraphicsDriver::ECulling::eBack);
			mBackEnd->setShadowVp(0, mShadowPass[0]->viewProj());
			mBackEnd->setShadowVp(1, mShadowPass[1]->viewProj());
			mBackEnd->setShadowVp(2, mShadowPass[2]->viewProj());
			mBackEnd->setShader(mShader);
			int uLightDir = mDriver->getUniformLocation("uLightDir");
			mDriver->setUniform(uLightDir, _context.lightDir());
			int uShadowMap0 = mDriver->getUniformLocation("uShadowMap0");
			mDriver->setUniform(uShadowMap0, mShadowPass[0]->tex());
			int uShadowMap1 = mDriver->getUniformLocation("uShadowMap1");
			mDriver->setUniform(uShadowMap1, mShadowPass[1]->tex());
			int uShadowMap2 = mDriver->getUniformLocation("uShadowMap2");
			mDriver->setUniform(uShadowMap2, mShadowPass[2]->tex());
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
			geom.uvs = _obj.mesh()->uvs;
			geom.nVertices = _obj.mesh()->nVertices;
			geom.transform = _obj.transform();
			//if (_obj.mMaterial)
			//	geom.color = _obj.mMaterial->mDiffuse;
			//else
				geom.color = Color(1.f, 1.f, 1.f);
			mBackEnd->render(geom);
		}

		//--------------------------------------------------------------------------------------------------------------
		Frustum ForwardRenderer::adjustShadowFrustum(const RenderContext& _context) {
			Mat34f invView;
			_context.camera()->view().inverse(invView);
			Frustum camF = _context.camera()->frustum();
			float minCaster = camF.farPlane();
			float maxCaster = camF.nearPlane();
			for (auto obj : _context) {
				if (obj->castShadows)
				{
					float objDist = (invView*obj->node()->position()).y;
					// Contribute to frustum adjustment
					minCaster = std::min(minCaster, objDist - obj->mBBox.radius());
					maxCaster = std::max(maxCaster, objDist + obj->mBBox.radius());
				}
			}
			float adjNear = std::max(camF.nearPlane(), minCaster);
			float adjFar = std::min(camF.farPlane(), maxCaster);
			if(adjFar < adjNear) // No casters in the frustum
				adjFar = adjNear;
			return Frustum(camF.aspectRatio(), camF.fov(), adjNear, adjFar);
		}*/
	}
}