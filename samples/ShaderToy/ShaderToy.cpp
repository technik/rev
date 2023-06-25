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

#include "ShaderToy.h"

#include <core/platform/cmdLineParser.h>
#include <core/platform/fileSystem/FolderWatcher.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <gfx/backend/Vulkan/vulkanCommandQueue.h>
#include <gfx/renderer/renderPass/fullScreenPass.h>
#include <gfx/Image.h>
#include <gfx/ImGui.h>
#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_win32.h>
#include <input/pointingInput.h>

#include <numbers>

using namespace rev::core;
using namespace rev::gfx;
using namespace rev::math;

namespace rev
{
	ShaderToy::ShaderToy()
	{}

	ShaderToy::~ShaderToy()
	{}

	void ShaderToy::getCommandLineOptions(CmdLineParser & options)
	{
		options.addOption("shader", &m_shaderFileName);
	}

	bool ShaderToy::init()
	{
		core::FileSystem::get()->registerPath("assets");
		core::FileSystem::get()->registerPath("shaders");

		// Create semaphore
		auto device = RenderContextVk().nativeDevice();
		m_imageAvailableSemaphore = device.createSemaphore({});

		m_timeVector = Vec4f::zero();
		m_frameBuffers = std::make_unique<FrameBufferManager>(RenderContextVk().nativeDevice());

		// Create descriptor layout
		m_descSetLayout = std::make_shared<DescriptorSetLayout>();
		m_descSetLayout->addTextureArray("Blue Noise", 0, (uint32_t)NumBlueNoiseTextures, vk::ShaderStageFlagBits::eFragment);
		m_descSetLayout->close();
		m_descSets = std::make_shared<DescriptorSetPool>(m_descSetLayout, 1);

		// Create render pass
		m_fullScreenFilter = std::make_unique<FullScreenPass>(
			m_shaderFileName,
			RenderContextVk().swapchainFormat(),
			*m_frameBuffers,
			m_descSetLayout->layout(),
			sizeof(PushConstants));

		initImGui(m_fullScreenFilter->vkPass());

		// Blue noise
		m_noisePermutations = std::uniform_int_distribution<unsigned>(0, NumBlueNoiseTextures - 1);
		loadNoiseTextures();

		// Update descriptors
		m_descSets->writeArrayTextureToDescriptor(0, "Blue Noise", m_blueNoise);

		// Create shader pipelines
		m_shaderWatcher = std::make_unique<core::FolderWatcher>(core::FolderWatcher::path("shaders"));
		m_shaderWatcher->listen([=](auto) {
			m_fullScreenFilter->invalidateShaders();
			});

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void ShaderToy::loadNoiseTextures()
	{
		std::string imageName = "blueNoise/LDR_RGBA_00.png";
		auto digitPos = imageName.find("00");
		m_blueNoise.resize(NumBlueNoiseTextures);
		for (unsigned i = 0; i < NumBlueNoiseTextures; ++i)
		{
			// Load image from file
			imageName[digitPos] = (i / 10) + '0';
			imageName[digitPos + 1] = (i % 10) + '0';

			auto image = Image4u8::load(imageName, false);
			if (image)
			{
				m_blueNoise[i] = RenderContextVk().allocator().createTexture("blue noise", Vec2u(64, 64),
					image->format(),
					vk::SamplerAddressMode::eRepeat,
					vk::SamplerAddressMode::eRepeat,
					false,
					1,
					image->data(),
					vk::ImageUsageFlagBits::eSampled,
					RenderContextVk().graphicsQueueFamily()
				);
			}
		}
	}

	bool ShaderToy::updateLogic(TimeDelta dt)
	{
		float t = m_timeVector.x();
		float T = m_timeVector.y();
		// Update time
		t += dt.count();
		T += dt.count();
		if (t > 1)
		{
			t -= 1.f;
		}
		m_timeVector = Vec4f(t, T, t * t, sin(std::numbers::pi_v<float> * t));
		return true;
	}

	void ShaderToy::render(TimeDelta dt)
	{
		Vec2f windowSize = { (float)RenderContext().windowSize().x(), (float)RenderContext().windowSize().y() };
		if (windowSize.x() == 0 || windowSize.y() == 0)
			return; // Don't try to render while minimized

		// Watch for shader reload
		m_shaderWatcher->update();

		// ImGui
		ImGui_ImplWin32_NewFrame();
		ImGui_ImplVulkan_NewFrame();

		auto& io = ImGui::GetIO();
		io.DisplaySize = { windowSize.x(), windowSize.y() };
		auto mousePos = input::PointingInput::get()->touchPosition();
		io.MousePos = { (float)mousePos.x(), (float)mousePos.y() };

		ImGui::NewFrame();

		if (ImGui::Begin("debug window"))
		{
			if (ImGui::CollapsingHeader("Options"))
			{
			}
		}
		ImGui::End();
		ImGui::Render();

		// Render passes
		auto cmd = RenderContextVk().getNewRenderCmdBuffer();
		cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));
		
		m_frameConstants.time = m_timeVector;
		m_fullScreenFilter->begin(
			cmd,
			RenderContext().windowSize(),
			RenderContextVk().swapchainAquireNextImage(m_imageAvailableSemaphore, cmd),
			nullptr,
			m_descSets->getDescriptor(0));

		m_fullScreenFilter->pushConstants(cmd, m_frameConstants);
		m_fullScreenFilter->render(cmd);

		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		m_fullScreenFilter->end(cmd);

		cmd.end();

		vk::PipelineStageFlags waitFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo(
			1, &m_imageAvailableSemaphore, &waitFlags, // wait
			1, &cmd, // commands
			1, &RenderContextVk().readyToPresentSemaphore()); // signal
		static_cast<VulkanCommandQueue&>(RenderContextVk().GfxQueue()).nativeQueue().submit(submitInfo);

		RenderContextVk().swapchainPresent();
	}
}