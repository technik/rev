//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "RenderPass.h"
#include <vulkan/vulkan.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <core/log.h>

namespace rev { namespace video {

	//----------------------------------------------------------------------------------------------------------------------
	RenderPass* RenderPass::create(NativeFrameBuffer& _renderTarget) {
		// Subpasses
		VkAttachmentReference colorAttachmentRef = {};
		colorAttachmentRef.attachment = 0;
		colorAttachmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

		VkAttachmentReference depthAttachmentRef = {};
		depthAttachmentRef.attachment = 1;
		depthAttachmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

		VkSubpassDescription subpass = {};
		subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
		subpass.colorAttachmentCount = 1;
		subpass.pColorAttachments = &colorAttachmentRef;
		subpass.pDepthStencilAttachment = &depthAttachmentRef;

		// Pass
		VkSubpassDependency dependency = {};
		dependency.srcSubpass = VK_SUBPASS_EXTERNAL;
		dependency.dstSubpass = 0;
		dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.srcAccessMask = 0;
		dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
		dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

		std::array<VkAttachmentDescription, 2> attachments = {_renderTarget.attachmentDescription(), _renderTarget.depthAttachmentDesc()};
		VkRenderPassCreateInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
		renderPassInfo.attachmentCount = 2;
		renderPassInfo.pAttachments = attachments.data();
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
	RenderPass::RenderPass(VkRenderPass _pass, NativeFrameBuffer& _renderTarget, VkCommandPool _commandPool, VkCommandBuffer _commandBuffer)
		: mVkPass(_pass)
		, mRenderTarget(_renderTarget)
		, mCommandPool(_commandPool)
		, mCommandBuffer(_commandBuffer)
	{
		// Config render target to use this pass
		_renderTarget.setRenderPass(mVkPass);

		// Set pipeline layout
		VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
		pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
		pipelineLayoutInfo.setLayoutCount = 0; // Optional
		pipelineLayoutInfo.pSetLayouts = nullptr; // Optional
		pipelineLayoutInfo.pushConstantRangeCount = 1; // Optional
		VkPushConstantRange range = {};
		range.size = 16*sizeof(float);
		range.offset = 0;
		range.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pipelineLayoutInfo.pPushConstantRanges = &range; // Optional

		if (vkCreatePipelineLayout(GraphicsDriver::get().device(), &pipelineLayoutInfo, nullptr, &mPipelineLayout) != VK_SUCCESS) {
			core::Log::error("failed to create pipeline layout!");
		}

		// Create pipeline
		setupVertexFormat();
		mPipeline = GraphicsDriver::get().createPipeline(
		{mRenderTarget.size().x, mRenderTarget.size().y}, mVkPass, mVertexFormat, mPipelineLayout);

		// Create synch semaphores
		VkSemaphoreCreateInfo semaphoreInfo = {};
		semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
		vkCreateSemaphore(GraphicsDriver::get().device(), &semaphoreInfo, nullptr, &mFinishedRenderingSemaphore);
	}

	//----------------------------------------------------------------------------------------------------------------------
	RenderPass::~RenderPass() {
		VkDevice device = GraphicsDriver::get().device();
		if(mPipeline) {
			vkDestroyPipeline(device, mPipeline, nullptr);
			vkDestroyPipelineLayout(device, mPipelineLayout, nullptr);
		}

		if(mCommandPool)
			vkDestroyCommandPool(device, mCommandPool, nullptr);

		vkDestroySemaphore(device, mFinishedRenderingSemaphore, nullptr);
		vkDestroyRenderPass(device, mVkPass, nullptr);
	}

	//----------------------------------------------------------------------------------------------------------------------
	void RenderPass::begin(RendererBackEnd& _backEnd) {
		// We must call RenderTarget::begin before we query it for the current image. Otherwise, it may return old frame buffers
		mRenderTarget.begin();

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
		renderPassInfo.framebuffer = mRenderTarget.activeBuffer();

		renderPassInfo.renderArea.offset = {0, 0};
		renderPassInfo.renderArea.extent = { mRenderTarget.size().x, mRenderTarget.size().y};

		std::array<VkClearValue, 2> clearValues = {};
		clearValues[0].color = {0.0f, 0.0f, 0.0f, 1.0f};
		clearValues[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = 2;
		renderPassInfo.pClearValues = clearValues.data();

		vkCmdBeginRenderPass(mCommandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

		// Bind Pipeline
		vkCmdBindPipeline(mCommandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS,mPipeline);
		_backEnd.mActivePipelineLayout = mPipelineLayout;
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

		mRenderTarget.end(mFinishedRenderingSemaphore);
	}

	//--------------------------------------------------------------------------------------------------------------
	void RenderPass::setupVertexFormat() {
		mVertexFormat.hasPosition = true;
		mVertexFormat.normalFmt = VertexFormat::UnitVecFormat::e3Vec3f;
		mVertexFormat.normalSpace = VertexFormat::NormalSpace::eModel;
		mVertexFormat.nUVs = 0;
	}

}}