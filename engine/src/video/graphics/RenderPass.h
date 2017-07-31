//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <vulkan/vulkan.h>
#include <video/graphics/driver/NativeFrameBuffer.h>
#include <video/graphics/renderer/backend/rendererBackEnd.h>

namespace rev { namespace video {

	class RenderPass {
	public:
		static RenderPass* create(const NativeFrameBuffer& _renderTarget);
		~RenderPass();
		void begin	(RendererBackEnd& _backEnd);
		void end	();

#ifdef REV_USE_VULKAN
		VkSemaphore	finishedRendering() const { return mFinishedRenderingSemaphore; }
#endif
		

	private:
		const NativeFrameBuffer& mRenderTarget;
#ifdef REV_USE_VULKAN
		RenderPass(VkRenderPass _pass, const NativeFrameBuffer& _renderTarget, VkCommandPool _commandPool, VkCommandBuffer _commandBuffer);
		VkRenderPass	mVkPass = VK_NULL_HANDLE;
		VkCommandPool	mCommandPool = VK_NULL_HANDLE;
		VkCommandBuffer	mCommandBuffer = VK_NULL_HANDLE;

		// Synchronization
		VkSemaphore mFinishedRenderingSemaphore;
#endif
	};

}}
