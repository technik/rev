//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <vulkan/vulkan.h>
#include <video/graphics/driver/NativeFrameBuffer.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>
#include <vector>

namespace rev { namespace video {

	class RenderPass {
	public:
		static RenderPass* create(NativeFrameBuffer& _renderTarget);
		~RenderPass	();
		void begin	(RendererBackEnd& _backEnd);
		void end	();

#ifdef REV_USE_VULKAN
		VkSemaphore	finishedRendering() const { return mFinishedRenderingSemaphore; }
#endif

	private:
		NativeFrameBuffer& mRenderTarget;
#ifdef REV_USE_VULKAN
		RenderPass(VkRenderPass _pass, NativeFrameBuffer& _renderTarget, VkCommandPool _commandPool, VkCommandBuffer _commandBuffer);
		void setupVertexFormat();

		VertexFormat		mVertexFormat;

		VkPipeline			mPipeline = VK_NULL_HANDLE;
		VkPipelineLayout	mPipelineLayout = VK_NULL_HANDLE;

		VkRenderPass		mVkPass = VK_NULL_HANDLE;
		VkCommandPool		mCommandPool = VK_NULL_HANDLE;
		VkCommandBuffer		mCommandBuffer = VK_NULL_HANDLE;

		// Synchronization
		VkSemaphore mFinishedRenderingSemaphore;
#endif
	};

}}
