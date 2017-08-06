//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/15
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer
#include "forwardRenderer.h"

#ifdef OPENGL_45
#include <core/components/affineTransform.h>
#include "../backend/rendererBackEnd.h"
#include <video/basicTypes/camera.h>
#include <video/graphics/renderScene.h>
#include <video/graphics/renderObj.h>
#include <video/graphics/staticRenderMesh.h>
#include <video/graphics/material.h>

using namespace rev::math;

#endif // OPENGL_45

#include <iostream>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/RenderPass.h>
#include <video/graphics/renderObj.h>

using namespace std;

namespace rev {
	namespace video {

		ForwardRenderer::~ForwardRenderer() {
			if(mRenderPass)
				delete mRenderPass;
		}

		//--------------------------------------------------------------------------------------------------------------
		bool ForwardRenderer::init(NativeFrameBuffer& _dstFrameBuffer) {
			mFrameBuffer = &_dstFrameBuffer;

			if(!createRenderPass()) {
				cout << "Forward renderer: failed to create render pass!\n";
				return false;
			}

			return true;
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::beginFrame() {
			GraphicsDriver::get().synch(); // Synch with Graphics
			mRenderPass->begin(mBackEnd); // Render forward pass
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::render(const RenderScene& _scene, const math::Mat44f& _vp) {
			// Uniform data
			for(auto obj : _scene.objects) 
			{
				math::Mat44f wvp = (_vp*obj->transform()).transpose(); // Transpose for vulkan
				// Send draw batches to back end
				RendererBackEnd::DrawCall callInfo = {};
				callInfo.nIndices = obj->mesh()->nIndices();
				callInfo.uniformData.size = sizeof(math::Mat44f);
				callInfo.uniformData.data = &wvp;
				//callInfo.mDescriptorSet = mDescriptorSet;
				RendererBackEnd::DrawBatch batch;
				batch.draws.push_back(callInfo);
				batch.indexBuffer = obj->mesh()->mIndexBuffer;
				batch.vertexBuffer = obj->mesh()->mVertexBuffer;
				mBackEnd.draw(batch);
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::endFrame() {
			// End render pass
			mRenderPass->end();
		}

		//--------------------------------------------------------------------------------------------------------------
		bool ForwardRenderer::createRenderPass() {
			mRenderPass = RenderPass::create(*mFrameBuffer);
			return nullptr != mRenderPass;
		}


#ifdef OPENGL_45
		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::init(GraphicsDriver* _driver) {
			mDriver = _driver;
			mBackEnd = new RendererBackEnd(_driver);
			mProgram = Shader::manager()->get("data\\pbr");
			mSkyboxShader = Shader::manager()->get("data\\skybox");
			mSkybox = Texture::loadFromFile("data\\skybox.pvr");
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::render(const std::vector<RenderObj*>& _scene, const Camera& _cam) const {
			// TODO: ShadowPass?
			// TODO: Sort into render queues based on material
			// Set global uniforms
			mDriver->beginFrame();
			//mDriver->setRenderTarget(_rt);
			mDriver->setViewport(Vec2i(0, 0), Vec2u(800, 600));
			mDriver->clearZBuffer();
			mDriver->clearColorBuffer();
			// Render state info
			RendererBackEnd::DrawCall	draw;
			RendererBackEnd::DrawInfo&	drawInfo = draw.renderStateInfo;
			drawInfo.program = mProgram;
			Vec3f globalLightDir = (Vec3f(0.8f, -1.f, -0.9f)).normalized();
			Vec3f viewPos = _cam.node()->component<core::AffineTransform>()->position();
			drawInfo.lightClr = Vec3f(1.f, 1.f, 0.9f)*1000.f;
			// Camera
			Mat34f invView = _cam.view().inverse();
			Mat44f viewProj = _cam.projection() * invView;
			// render objects
			for (const auto obj : _scene) {
				Mat34f modelMatrix = obj->node()->component<core::AffineTransform>()->matrix();
				Mat34f invModelMtx = modelMatrix.inverse();

				drawInfo.wvp = viewProj * modelMatrix;
				drawInfo.lightDir = invModelMtx.rotate(globalLightDir);
				drawInfo.viewPos = invModelMtx * viewPos;

				if (obj->material) {
					for (const auto& m : obj->material->mMaps)
						drawInfo.texUniforms.push_back(m);
					if (mSkybox)
						drawInfo.texUniforms.push_back(std::make_pair("environmentMap", mSkybox));
					for (const auto& v3 : obj->material->mVec3s)
						drawInfo.vec3Uniforms.push_back(v3);
					for (const auto& f : obj->material->mFloats)
						drawInfo.floatUniforms.push_back(f);
				}

				draw.mesh = obj->mesh();
				mBackEnd->draw(draw);
				draw.renderStateInfo.texUniforms.clear();
				draw.renderStateInfo.vec3Uniforms.clear();
				draw.renderStateInfo.floatUniforms.clear();
			}
			// Draw skybox cubemap
			drawSkyboxCubemap(_cam);
			mBackEnd->flush();
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::drawSkyboxCubemap(const Camera& _cam) const {
			RendererBackEnd::DrawInfo cubeMapInfo;
			cubeMapInfo.program = mSkyboxShader;
			cubeMapInfo.texUniforms.push_back(std::make_pair("albedo", mSkybox));
			Mat34f centeredView = _cam.view();
			centeredView.setCol(3, Vec3f(0));
			cubeMapInfo.wvp = _cam.projection() * centeredView.inverse();
			mBackEnd->drawSkybox(cubeMapInfo);
		}
#endif // OPENGL_45
	}
}
