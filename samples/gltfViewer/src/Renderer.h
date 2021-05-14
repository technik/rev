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
#include <graphics/backend/DescriptorSet.h>
#include <graphics/backend/FrameBufferManager.h>
#include <graphics/backend/rasterPipeline.h>
#include <graphics/backend/Vulkan/gpuBuffer.h>
#include <graphics/backend/Vulkan/renderContextVulkan.h>
#include <graphics/RasterHeap.h>
#include <graphics/scene/Material.h>
#include <graphics/Texture.h>

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
			std::vector<gfx::PBRMaterial> m_materials;
			std::vector<std::shared_ptr<gfx::Texture>> m_textures;

			math::Mat44f proj;
			math::Mat44f view;
			math::Vec3f lightDir;
			math::Vec3f ambientColor;
			math::Vec3f lightColor;
		};

		size_t init(
			gfx::RenderContextVulkan& ctxt,
			const math::Vec2u& windowSize,
			const SceneDesc& scene
		);
		void end();
		void onResize(const math::Vec2u& windowSize);
		void render(SceneDesc& scene, bool geometryReady);
		void updateUI();

	private:
		void createDescriptorLayouts(size_t numTextures);
		void fillConstantDescriptorSets(const SceneDesc& scene);
		void createRenderPasses();
		void createShaderPipelines();
		void createRenderTargets();
		void destroyRenderTargets();
		void destroyFrameBuffers();
		void loadIBLLUT();

		// Init ImGui
		void initImGui();
		bool renderFlag(uint32_t flag) const { return (m_frameConstants.renderFlags & flag) > 0; }

	private:
		gfx::RenderContextVulkan* m_ctxt;
		math::Vec2u m_windowSize;
		vk::Viewport m_viewport;
		vk::Rect2D m_renderArea;

		// Vulkan objects to move into rev::gfx
		vk::Semaphore m_imageAvailableSemaphore;
		uint32_t m_doubleBufferNdx = 0;
		gfx::DescriptorSetLayout m_geometryDescriptorSets;
		gfx::DescriptorSetLayout m_postProDescriptorSets;

		vk::PipelineLayout m_gbufferPipelineLayout;
		vk::PipelineLayout m_postProcessPipelineLayout;
		std::unique_ptr<gfx::RasterPipeline> m_gBufferPipeline;
		std::unique_ptr<gfx::RasterPipeline> m_postProPipeline;
		std::shared_ptr<gfx::ImageBuffer> m_gBufferNormals;
		std::shared_ptr<gfx::ImageBuffer> m_gBufferPBR;
		std::shared_ptr<gfx::ImageBuffer> m_gBufferZ;

		std::vector<std::shared_ptr<gfx::GPUBuffer>> m_mtxBuffers;
		std::shared_ptr<gfx::GPUBuffer> m_materialsBuffer;
		std::shared_ptr<gfx::Texture> m_iblLUT;

		struct FramePushConstants
		{
			math::Mat44f proj;
			math::Mat44f view;
			math::Vec3f lightDir;
			math::Vec3f ambientColor;
			math::Vec3f lightColor;

			uint32_t renderFlags = 0;
			// Material override parameters
			math::Vec3f overrideBaseColor = { 0.7f, 0.7f, 0.7f };
			float overrideMetallic = 0.f;
			float overrideRoughness = 0.75f;
			float overrideClearCoat = 0.f;

		} m_frameConstants;

		struct PostProPushConstants
		{
			uint32_t renderFlags;
			math::Vec2f windowSize;
			float exposure;
			float bloom;
		};

		std::unique_ptr<RenderPass> m_uiRenderPass;
		std::unique_ptr<RenderPass> m_gBufferPass;

		std::unique_ptr<core::FolderWatcher> m_shaderWatcher;
		std::unique_ptr<gfx::FrameBufferManager> m_frameBuffers;
	};
}