//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple forward renderer
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_

#ifdef OPENGL_45
#include <vector>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/shader/shader.h>
#include <video/basicTypes/texture.h>
#endif

#include <video/graphics/driver/NativeFrameBuffer.h>
#include <video/graphics/geometry/RenderGeom.h>

#ifdef REV_USE_VULKAN
#include <vulkan/vulkan.h>
#include <vulkan/vulkan.hpp>
#endif // REV_USE_VULKAN

namespace rev {
	namespace video {

		class ForwardRenderer {
		public:
			~ForwardRenderer();
			// Set up the renderer to render into the given frame buffer
			bool init(const NativeFrameBuffer&);

			void beginFrame();
			void render(const RenderGeom&);
			void endFrame();

#ifdef REV_USE_VULKAN
			VkPipeline	pipeline() const { return mPipeline; }
			VkRenderPass renderPass() const { return mRenderPass; }

		private:
			bool createRenderPass();
			bool createDescriptorSetLayout();
			bool createPipeline(const VkExtent2D& _viewportSize);
			bool createFrameBufferViews();
			bool createDescriptorPool();
			bool createDescriptorSet();

		private:
			const NativeFrameBuffer* mFrameBuffer;
			uint32_t mCurFBImageIndex;

			VkDevice		mDevice = VK_NULL_HANDLE;
			VkRenderPass	mRenderPass = VK_NULL_HANDLE;
			VkPipeline		mPipeline = VK_NULL_HANDLE;
			VkDescriptorSetLayout mDescriptorSetLayout;
			VkPipelineLayout mPipelineLayout = VK_NULL_HANDLE;
			VkCommandPool	mCommandPool = VK_NULL_HANDLE;
			VkCommandBuffer	mCommandBuffer = VK_NULL_HANDLE;
			std::vector<VkFramebuffer>	mSwapChainFramebuffers;
			VkDescriptorPool mDescriptorPool;
			VkDescriptorSet mDescriptorSet;

			vk::Buffer mUniformBuffer;
			vk::DeviceMemory mUniformBufferMemory;

			VkSemaphore imageAvailableSemaphore;
			VkSemaphore renderFinishedSemaphore;

#endif // REV_USE_VULKAN

#ifdef OPENGL_45
			void init(GraphicsDriver* _driver);
			// TODO: Instead of a camera, this should receive a "render info" struct with
			// information like: use shadows? LOD bias? cam, etc
			// nullptr render target means: use framebuffer
			void render(const std::vector<RenderObj*>& _scene, const Camera& _cam) const;

		private:
			void drawSkyboxCubemap(const Camera& _cam) const;

			Texture*			mSkybox;
			Shader::Ptr			mSkyboxShader;

			GraphicsDriver*		mDriver = nullptr;
			RendererBackEnd*	mBackEnd = nullptr;
			Shader::Ptr			mProgram = nullptr;
#endif // OPENGL_45
		};
	}
}


#endif // _REV_VIDEO_GRAPHICS_RENDERER_FORWARD_FORWARDRENDERER_H_