//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once

#include <core/platform/fileSystem/FolderWatcher.h>
#include <game/scene/meshRenderer.h>
#include <graphics/backend/rasterPipeline.h>
#include <graphics/backend/Vulkan/gpuBuffer.h>
#include <graphics/backend/Vulkan/renderContextVulkan.h>
#include <graphics/RasterHeap.h>

#include "RenderPass.h"

namespace rev
{
	namespace gfx
	{
		class RenderContextVulkan;
	}

	class Renderer
	{
	public:
		struct SceneDesc
		{
			game::MeshRenderer m_sceneInstances;
			gfx::RasterHeap m_rasterData;

			math::Mat44f proj;
			math::Mat44f view;
			math::Vec4f lightDir;
			math::Vec4f ambientColor;
			math::Vec4f lightColor;
		};

		void init(gfx::RenderContextVulkan& ctxt, const math::Vec2u& windowSize, size_t maxNumInstances);
		void end();
		void onResize(const math::Vec2u& windowSize);
		void render(SceneDesc& scene, bool geometryReady);

	private:
		void createFrameBuffers();
		void destroyFrameBuffers();

		// Init ImGui
		void setRenderArea(const math::Vec2u& size);
		void initImGui();

	private:
		gfx::RenderContextVulkan* m_ctxt;
		math::Vec2u m_windowSize;
		vk::Viewport m_viewport;
		vk::Rect2D m_renderArea;

		// Vulkan objects to move into rev::gfx
		vk::Semaphore m_imageAvailableSemaphore;
		vk::RenderPass m_gBufferPass;
		vk::RenderPass m_uiPass;
		vk::DescriptorPool m_descPool;
		std::vector<vk::DescriptorSet> m_frameDescs;
		uint32_t m_doubleBufferNdx = 0;
		vk::DescriptorSetLayout m_frameDescLayout;
		vk::PipelineLayout m_gbufferPipelineLayout;
		std::unique_ptr<gfx::RasterPipeline> m_gBufferPipeline;
		std::shared_ptr<gfx::ImageBuffer> m_gBufferNormals;
		std::shared_ptr<gfx::ImageBuffer> m_gBufferPBR;
		std::shared_ptr<gfx::ImageBuffer> m_gBufferZ;
		std::vector<vk::Framebuffer> m_swapchainFrameBuffers;
		std::vector<std::shared_ptr<gfx::GPUBuffer>> m_mtxBuffers;
		size_t m_maxNumInstances;

		struct FramePushConstants
		{
			math::Mat44f proj;
			math::Mat44f view;
			math::Vec4f lightDir;
			math::Vec4f ambientColor;
			math::Vec4f lightColor;
		} m_frameConstants;

		RenderPass m_uiRenderPass;

		std::unique_ptr<core::FolderWatcher> m_shaderWatcher;
	};
}