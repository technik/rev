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
#include <gfx/backend/DescriptorSet.h>
#include <gfx/backend/Vulkan/Vulkan.h>
#include <gfx/renderer/RenderPass.h>
#include <gfx/renderer/EnvironmentProbe.h>
#include <math/algebra/matrix.h>

namespace rev::gfx
{
	class FrameBufferManager;
	class GPUBuffer;
	class RasterPipeline;
	class RenderContextVulkan;
	class FullScreenPass;
	class RasterQueue;

	class DeferredRenderer
	{
	public:
		struct SceneDesc
		{
			std::vector<std::shared_ptr<RasterQueue>> m_opaqueGeometry;

			math::Mat44f proj;
			math::Mat44f view;
			math::Vec3f lightDir;
			math::Vec3f ambientColor;
			math::Vec3f lightColor;
		};

		struct Budget
		{
			uint32_t maxTexturesPerBatch;
		};

		DeferredRenderer();
		~DeferredRenderer();

		void init(
			gfx::RenderContextVulkan& ctxt,
			const math::Vec2u& windowSize,
			const Budget& limits,
			const core::FolderWatcher::path& shadersFolder,
			std::shared_ptr<EnvironmentProbe> envProbe
		);
		void end();
		void onResize(const math::Vec2u& windowSize);
		void render(SceneDesc& scene);
		void updateUI();
		auto batchDescriptorLayout() const { return m_geomBatchDescriptorLayout; }

	private:
		void createDescriptorLayouts(size_t numTextures);
		void fillConstantDescriptorSets();
		void createRenderPasses();
		void createShaderPipelines();
		void createRenderTargets();
		void destroyRenderTargets();
		void destroyFrameBuffers();
		void loadIBLLUT();

		void renderGeometryPass(SceneDesc& scene);
		void renderLightingPass();
		void renderPostProPass();

		// Init ImGui
		bool renderFlag(uint32_t flag) const { return (m_frameConstants.renderFlags & flag) > 0; }

	private:
		gfx::RenderContextVulkan* m_ctxt;
		math::Vec2u m_windowSize;
		vk::Viewport m_viewport;
		vk::Rect2D m_renderArea;

		// Vulkan objects to move into rev::gfx
		vk::Semaphore m_imageAvailableSemaphore;
		vk::Semaphore m_renderGeometrySemaphore;
		uint32_t m_doubleBufferNdx = 0;

		std::shared_ptr<gfx::DescriptorSetLayout> m_geomFrameDescriptorLayout;
		std::shared_ptr<gfx::DescriptorSetLayout> m_geomBatchDescriptorLayout;
		std::shared_ptr<gfx::DescriptorSetLayout> m_lightingDescriptorLayout;
		std::shared_ptr<gfx::DescriptorSetLayout> m_postProDescriptorLayout;
		std::shared_ptr<gfx::DescriptorSetPool> m_geomFrameDescriptors;
		std::shared_ptr<gfx::DescriptorSetPool> m_lightingDescriptors;
		std::shared_ptr<gfx::DescriptorSetPool> m_postProDescriptors;

		vk::PipelineLayout m_gbufferPipelineLayout;
		std::unique_ptr<gfx::RasterPipeline> m_gBufferPipeline;
		vk::PipelineLayout m_lightingPipelineLayout;
		std::unique_ptr<gfx::RasterPipeline> m_lightingPipeline;
		vk::PipelineLayout m_postPipelineLayout;
		std::unique_ptr<gfx::RasterPipeline> m_postPipeline;

		std::shared_ptr<gfx::ImageBuffer> m_hdrLightBuffer;
		std::shared_ptr<gfx::ImageBuffer> m_emissiveBuffer;
		std::shared_ptr<gfx::ImageBuffer> m_baseColorMetalnessBuffer;
		std::shared_ptr<gfx::ImageBuffer> m_normalPBRBuffer;
		std::shared_ptr<gfx::ImageBuffer> m_zBuffer;

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

		struct LightingPushConstants
		{
			uint32_t renderFlags;
			math::Vec2f windowSize;
			math::Vec3f lightDir;
			math::Vec3f lightColor;
			math::Vec3f ambientColor;
		} m_lightingConstants;

		struct PostProPushConstants
		{
			math::Vec3f ambientColor;
			uint32_t renderFlags;

			math::Vec2f windowSize;
			float exposure = 1.f;
			float bloom;
		} m_postProConstants;

		std::unique_ptr<gfx::RenderPass> m_gBufferPass;
		std::unique_ptr<gfx::FullScreenPass> m_lightingPass;
		std::unique_ptr<gfx::FullScreenPass> m_postPass; // Combined post process effects
		std::shared_ptr<EnvironmentProbe> m_envProbe;

		static constexpr vk::Format m_HDRFormat = vk::Format::eR16G16B16A16Sfloat;

		std::unique_ptr<core::FolderWatcher> m_shaderWatcher;
		std::unique_ptr<gfx::FrameBufferManager> m_frameBuffers;

		core::FolderWatcher::path m_shadersFolder;
	};
}