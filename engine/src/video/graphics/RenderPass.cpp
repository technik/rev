//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "RenderPass.h"
#include <vulkan/vulkan.h>
#include <video/graphics/driver/graphicsDriver.h>

namespace rev { namespace video {

	//----------------------------------------------------------------------------------------------------------------------
	RenderPass* RenderPass::create(const NativeFrameBuffer& _renderTarget) {
		// Subpasses
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;

		// Pass
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 1;
		renderPassInfo.pAttachments = &_renderTarget.attachmentDescription();
		renderPassInfo.subpassCount = 1;
		renderPassInfo.pSubpasses = &subpass;
		renderPassInfo.dependencyCount = 1;
		renderPassInfo.pDependencies = &dependency;

		VkRenderPass vkPass;
		if(VK_SUCCESS != vkCreateRenderPass(GraphicsDriver::get().device(), &renderPassInfo, nullptr, &vkPass)) {
			return nullptr;
		}

		// Command buffer
		VkCommandPool commandPool = GraphicsDriver::get().createCommandPool(true); // The command buffers get reset every frame

		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.commandPool = commandPool;
		allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
		allocInfo.commandBufferCount = 1;

		VkCommandBuffer commandBuffer;
		if (vkAllocateCommandBuffers(GraphicsDriver::get().device(), &allocInfo, &commandBuffer) != VK_SUCCESS) {
			return nullptr;
		}

		return new RenderPass(vkPass, _renderTarget, commandPool, commandBuffer);
	}

	//----------------------------------------------------------------------------------------------------------------------
	RenderPass::RenderPass(VkRenderPass _pass, const NativeFrameBuffer& _renderTarget, VkCommandPool _commandPool, VkCommandBuffer _commandBuffer)
		: mVkPass(_pass)
		, mRenderTarget(_renderTarget)
		, mCommandPool(_commandPool)
		, mCommandBuffer(_commandBuffer)
	{
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkCreateSemaphore(GraphicsDriver::get().device(), &semaphoreInfo, nullptr, &mFinishedRenderingSemaphore);
	}

	//----------------------------------------------------------------------------------------------------------------------
	RenderPass::~RenderPass() {
		if(mCommandPool)
			vkDestroyCommandPool(GraphicsDriver::get().device(), mCommandPool, nullptr);

		vkDestroyRenderPass(GraphicsDriver::get().device(), mVkPass, nullptr);
	}

	//----------------------------------------------------------------------------------------------------------------------
	void RenderPass::begin(RendererBackEnd& _backEnd) {
		// ----- Record command buffer -----
		// Begin command buffer
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
		beginInfo.pInheritanceInfo = nullptr; // Optional

		vkBeginCommandBuffer(mCommandBuffer, &beginInfo);

		// Configure back end to render into my command buffer
		_backEnd.setTargetCommandBuffer(mCommandBuffer);

		// Bind render pass
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = mVkPass;
		renderPassInfo.framebuffer = mRenderTarget.activeFrameBuffer();

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = { mRenderTarget.size().x, mRenderTarget.size().y};

		VkClearValue clearColor = {0.0f, 0.0f, 0.0f, 1.0f};
		renderPassInfo.clearValueCount = 1;
		renderPassInfo.pClearValues = &clearColor;

		vkCmdBeginRenderPass(mCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
	}

	//----------------------------------------------------------------------------------------------------------------------
	void RenderPass::end() {
		vkCmdEndRenderPass(mCommandBuffer);
		vkEndCommandBuffer(mCommandBuffer);

		// ----- Submit command buffer -----
		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

		VkSemaphore waitSemaphores[] = { mRenderTarget.imageAvailable() };
		VkPipelineStageFlags waitStages[] = { VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT };
		submitInfo.waitSemaphoreCount = 1;
		submitInfo.pWaitSemaphores = waitSemaphores;
		submitInfo.pWaitDstStageMask = waitStages;

		submitInfo.commandBufferCount = 1;
		submitInfo.pCommandBuffers = &mCommandBuffer;

		VkSemaphore signalSemaphores[] = {mFinishedRenderingSemaphore};
		submitInfo.signalSemaphoreCount = 1;
		submitInfo.pSignalSemaphores = signalSemaphores;

		vkQueueSubmit(GraphicsDriver::get().graphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
	}

}}