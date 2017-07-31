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

#ifdef REV_USE_VULKAN
#include <iostream>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/RenderPass.h>

using namespace std;

#endif // REV_USE_VULKAN

namespace rev {
	namespace video {

#ifdef REV_USE_VULKAN
		ForwardRenderer::~ForwardRenderer() {
			if(!mDevice)
				return;

			vkDestroySemaphore(mDevice, renderFinishedSemaphore, nullptr);

			for (size_t i = 0; i < mSwapChainFramebuffers.size(); i++) {
				vkDestroyFramebuffer(mDevice, mSwapChainFramebuffers[i], nullptr);
			}

			if(mPipeline) {
				vkDestroyPipeline(mDevice, mPipeline, nullptr);
				vkDestroyPipelineLayout(mDevice, mPipelineLayout, nullptr);
			}

			vkDestroyDescriptorSetLayout(mDevice, mDescriptorSetLayout, nullptr);

			if(mRenderPass)
				delete mRenderPass;
		}

		//--------------------------------------------------------------------------------------------------------------
		bool ForwardRenderer::init(const NativeFrameBuffer& _dstFrameBuffer) {
			mDevice = GraphicsDriver::get().device(); // Vulkan device
			mFrameBuffer = &_dstFrameBuffer;
			assert(mDevice);

			if(!createRenderPass()) {
				cout << "Forward renderer: failed to create render pass!\n";
				return false;
			}

			if(!createDescriptorSetLayout()) {
				cout << "Failed to create descriptor set layout\n";
				return false;
			}

			auto viewportSize = mFrameBuffer->size();
			mPipeline = GraphicsDriver::get().createPipeline({viewportSize.x, viewportSize.y }, mRenderPass, mVertexFormat, mPipelineLayout);
			if(mPipeline == VK_NULL_HANDLE) {
				cout << "failed to create graphics pipeline!";
				return false;
			}

			if(!createFrameBufferViews()) {
				cout << "failed to create frame buffer views\n";
				return false;
			}

			// Allocate uniform buffer
			//VkDeviceSize bufferSize = sizeof(math::Vec2f);
			//GraphicsDriver::get().createBuffer(bufferSize, vk::BufferUsageFlagBits::eUniformBuffer, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent, mUniformBuffer, mUniformBufferMemory);

			//createDescriptorPool();
			//createDescriptorSet();

			return true;
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::beginFrame() {
			// Get target image from the swapchain
			mRenderPass->begin();
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::render(const RenderGeom& _geom, const math::Mat44f& _wvp) {
			// Pipeline
			vkCmdBindPipeline(mBackEnd.mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,mPipeline);
			mBackEnd.mActivePipelineLayout = mPipelineLayout;
			// Send draw batches to back end
			RendererBackEnd::DrawCall callInfo = {};
			callInfo.nIndices = _geom.nIndices();
			callInfo.uniformData.size = sizeof(math::Mat44f);
			callInfo.uniformData.data = &_wvp;
			//callInfo.mDescriptorSet = mDescriptorSet;
			RendererBackEnd::DrawBatch batch;
			batch.draws.push_back(callInfo);
			batch.indexBuffer = _geom.mIndexBuffer;
			batch.vertexBuffer = _geom.mVertexBuffer;
			mBackEnd.draw(batch);
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::endFrame() {
			// End render pass
			mRenderPass->end();

			// ----- Submit command buffer -----
			VkSubmitInfo submitInfo = {};
			submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

			VkSemaphore waitSemaphores[] = {imageAvailableSemaphore};
			VkPipelineStageFlags waitStages[] = {VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT};
			submitInfo.waitSemaphoreCount = 1;
			submitInfo.pWaitSemaphores = waitSemaphores;
			submitInfo.pWaitDstStageMask = waitStages;

			submitInfo.commandBufferCount = 1;
			submitInfo.pCommandBuffers = &mBackEnd.mCommandBuffer;

			VkSemaphore signalSemaphores[] = {renderFinishedSemaphore};
			submitInfo.signalSemaphoreCount = 1;
			submitInfo.pSignalSemaphores = signalSemaphores;

			vkQueueSubmit(GraphicsDriver::get().graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);

			// Present
			VkPresentInfoKHR presentInfo = {};
			presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;

			presentInfo.waitSemaphoreCount = 1;
			presentInfo.pWaitSemaphores = signalSemaphores;

			VkSwapchainKHR swapChains[] = {(VkSwapchainKHR) mFrameBuffer->swapChain() };
			presentInfo.swapchainCount = 1;
			presentInfo.pSwapchains = swapChains;
			presentInfo.pImageIndices = &mCurFBImageIndex;

			presentInfo.pResults = nullptr; // Optional
			vkQueuePresentKHR(GraphicsDriver::get().graphicsQueue(), &presentInfo);

			vkQueueWaitIdle(GraphicsDriver::get().graphicsQueue());
		}

		//--------------------------------------------------------------------------------------------------------------
		bool ForwardRenderer::createRenderPass() {
			mRenderPass = RenderPass::create(*mFrameBuffer);
			return nullptr != mRenderPass;
		}

		//--------------------------------------------------------------------------------------------------------------
		bool ForwardRenderer::createDescriptorSetLayout() {
			VkDescriptorSetLayoutBinding uboLayoutBinding = {};
			uboLayoutBinding.binding = 0;
			uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			uboLayoutBinding.descriptorCount = 1;
			uboLayoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
			uboLayoutBinding.pImmutableSamplers = nullptr;

			VkDescriptorSetLayoutCreateInfo layoutInfo = {};
			layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
			layoutInfo.bindingCount = 1;
			layoutInfo.pBindings = &uboLayoutBinding;

			if (vkCreateDescriptorSetLayout(mDevice, &layoutInfo, nullptr, &mDescriptorSetLayout) != VK_SUCCESS) {
				cout << "failed to create descriptor set layout!\n";
				return false;
			}

			return true;
		}

		//--------------------------------------------------------------------------------------------------------------
		void ForwardRenderer::setupVertexFormat() {
			mVertexFormat.hasPosition = true;
			mVertexFormat.normalFmt = VertexFormat::UnitVecFormat::e3Vec3f;
			mVertexFormat.normalSpace = VertexFormat::NormalSpace::eModel;
			mVertexFormat.nUVs = 0;
		}

		//--------------------------------------------------------------------------------------------------------------
		bool ForwardRenderer::createDescriptorPool() {
			VkDescriptorPoolSize poolSize = {};
			poolSize.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			poolSize.descriptorCount = 1;

			VkDescriptorPoolCreateInfo poolInfo = {};
			poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
			poolInfo.poolSizeCount = 1;
			poolInfo.pPoolSizes = &poolSize;

			poolInfo.maxSets = 1;

			if (vkCreateDescriptorPool(mDevice, &poolInfo, nullptr, &mDescriptorPool) != VK_SUCCESS) {
				return false;
			}

			return true;
		}

		//--------------------------------------------------------------------------------------------------------------
		bool ForwardRenderer::createDescriptorSet() {
			VkDescriptorSetLayout layouts[] = {mDescriptorSetLayout};
			VkDescriptorSetAllocateInfo allocInfo = {};
			allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
			allocInfo.descriptorPool = mDescriptorPool;
			allocInfo.descriptorSetCount = 1;
			allocInfo.pSetLayouts = layouts;

			if (vkAllocateDescriptorSets(mDevice, &allocInfo, &mDescriptorSet) != VK_SUCCESS) {
				return false;
			}

			VkDescriptorBufferInfo bufferInfo = {};
			bufferInfo.buffer = (VkBuffer)mUniformBuffer;
			bufferInfo.offset = 0;
			bufferInfo.range = sizeof(math::Vec2f);

			VkWriteDescriptorSet descriptorWrite = {};
			descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
			descriptorWrite.dstSet = mDescriptorSet;
			descriptorWrite.dstBinding = 0;
			descriptorWrite.dstArrayElement = 0;

			descriptorWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
			descriptorWrite.descriptorCount = 1;
			descriptorWrite.pBufferInfo = &bufferInfo;
			vkUpdateDescriptorSets(mDevice, 1, &descriptorWrite, 0, nullptr);

			return true;
		}

#endif // REV_USE_VULKAN

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

		//--------------------------------------------------------------------------------------------------------------
/*		void ForwardRenderer::init(GraphicsDriver* _driver) {
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
#endif // OPENGL_45
	}
}
