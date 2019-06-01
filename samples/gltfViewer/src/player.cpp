//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "player.h"

#include <core/platform/cmdLineParser.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <core/time/time.h>
#include <graphics/backend/commandPool.h>
#include <graphics/backend/device.h>
#include <graphics/backend/DirectX12/directX12Driver.h>
#include <graphics/backend/doubleBufferSwapChain.h>
#include <graphics/shaders/shaderCodeFragment.h>
#include <math/algebra/vector.h>
#include <math/algebra/matrix.h>
#include <iostream>

using namespace rev::math;
using namespace rev::gfx;
using namespace rev::game;

namespace rev {

	//------------------------------------------------------------------------------------------------------------------
	Player::CmdLineOptions::CmdLineOptions(int _argc, const char** _argv)
	{
		core::CmdLineParser argParser;
		argParser.addArgument("sx", [this](const char* sx) { windowSize.x() = atoi(sx); });
		argParser.addArgument("sy", [this](const char* sy) { windowSize.y() = atoi(sy); });
		argParser.addArgument("scene", [this](const char* _scene) { scene = _scene; });
		argParser.addFlag("fullHD", [this]() { windowSize = { 1920, 1980 }; });

		argParser.parse(_argc, _argv);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::initGraphicsDevice()
	{
		// Init graphics
		DirectX12Driver gfxDriver;
		const int MAX_GRAPHICS_CARDS = 5;
		GraphicsDriver::PhysicalDeviceInfo physicalDevices[MAX_GRAPHICS_CARDS];
		int numGfxCards = gfxDriver.enumeratePhysicalDevices(physicalDevices, MAX_GRAPHICS_CARDS);

		size_t maxVideoMemory = 0;
		int bestDevice = -1;
		for (int i = 0; i < numGfxCards; ++i)
		{
			if (physicalDevices[i].dedicatedVideoMemory > maxVideoMemory)
			{
				bestDevice = i;
				maxVideoMemory = physicalDevices[i].dedicatedVideoMemory;
			}
		}

		if (bestDevice < 0)
		{
			std::cout << "Unable to find a suitable graphics card\n";
			return false;
		}

		GraphicsDriver::PhysicalDevice* gfxCard = gfxDriver.createPhysicalDevice(bestDevice);

		// Create one command queue of each type, all with the same prioriry
		CommandQueue::Info commandQueues[3];
		commandQueues[0].type = CommandQueue::Graphics;
		commandQueues[0].priority = CommandQueue::Normal;
		commandQueues[1].type = CommandQueue::Compute;
		commandQueues[1].priority = CommandQueue::Normal;
		commandQueues[2].type = CommandQueue::Copy;
		commandQueues[2].priority = CommandQueue::Normal;

		m_gfxDevice = gfxDriver.createDevice(*gfxCard, 3, commandQueues);
		if (!m_gfxDevice)
		{
			std::cout << "Unable to create graphics device\n";
			return false;
		}

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createSwapChain()
	{
		auto nativeWindow = rev::gfx::createWindow(
			{ 80, 80 },
			m_windowSize,
			"Rev Player",
			true,
			true // Visible
		);

		DoubleBufferSwapChain::Info swapChainInfo;
		swapChainInfo.pixelFormat.channel = Image::ChannelFormat::Byte;
		swapChainInfo.pixelFormat.numChannels = 4;
		swapChainInfo.size = m_windowSize;

		m_swapChain = m_gfxDevice->createSwapChain(nativeWindow, 0, swapChainInfo);

		m_backBuffers[0] = m_swapChain->backBuffer(0);
		m_backBuffers[1] = m_swapChain->backBuffer(1);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(const CmdLineOptions& options)
	{
		core::Time::init();
		
		rev::core::FileSystem::init();

		m_windowSize = options.windowSize;
		initGraphicsDevice();
		createSwapChain();

		// Create two command pools, for alternate frames
		int backBufferIndex = 0;
		m_frameCmdPools[0] = m_gfxDevice->createCommandPool(CommandList::Graphics);
		m_frameCmdPools[1] = m_gfxDevice->createCommandPool(CommandList::Graphics);
		m_frameCmdList = m_gfxDevice->createCommandList(CommandList::Graphics, *m_frameCmdPools[0]);
		m_frameCmdList->close(); // Close immediately so that we can immediately reset it at the beginning of the first frame
		m_frameFence = m_gfxDevice->createFence();

		// Create command list for copying data
		m_copyCommandPool = m_gfxDevice->createCommandPool(CommandList::Copy);
		
		loadScene(options.scene);

		return true;
	}

#ifdef _WIN32
	//------------------------------------------------------------------------------------------------------------------
	void Player::onWindowResize(const math::Vec2u& _newSize)
	{
		m_windowSize = _newSize;
		//mForwardRenderer.onResizeTarget(_newSize);
		//mDeferred.onResizeTarget(_newSize);
	}
#endif // _WIN32

	//------------------------------------------------------------------------------------------------------------------
	void Player::loadScene(const std::string& scene)
	{
		Fence* sceneLoadFence = m_gfxDevice->createFence();
		CommandList* copyCmdList = m_gfxDevice->createCommandList(CommandList::Copy, *m_copyCommandPool);

		// Create buffers for vertex data
		auto* stagingBuffer = m_gfxDevice->createCommitedResource(Device::BufferType::Upload, Device::ResourceFlags::None, 3 * sizeof(rev::math::Vec3f));
		m_sceneVertexBuffer = m_gfxDevice->createCommitedResource(Device::BufferType::Resident, Device::ResourceFlags::None, 3 * sizeof(rev::math::Vec3f));

		rev::math::Vec3f vtx[3] = {
			rev::math::Vec3f(0.5f,0.1f,0.5f),
			rev::math::Vec3f(0.f,-1.f,0.f),
			rev::math::Vec3f(-0.5f,0.f,0.f)
		};
		copyCmdList->uploadBufferContent(*m_sceneVertexBuffer, *stagingBuffer, 3 * sizeof(rev::math::Vec3f), vtx);

		// Create buffers for index data
		auto* indexStagingBuffer = m_gfxDevice->createCommitedResource(Device::BufferType::Upload, Device::ResourceFlags::None, 3 * sizeof(uint16_t));
		m_sceneIndexBuffer = m_gfxDevice->createCommitedResource(Device::BufferType::Resident, Device::ResourceFlags::None, 3 * sizeof(uint16_t));

		uint16_t indices[3] = { 0, 1, 2 };
		copyCmdList->uploadBufferContent(*m_sceneIndexBuffer, *indexStagingBuffer, 3 * sizeof(uint16_t), indices);

		// Execute copy on a command queue
		copyCmdList->close();
		auto& copyQueue = m_gfxDevice->commandQueue(2);
		copyQueue.executeCommandList(*copyCmdList);
		auto copyFenceValue = copyQueue.signalFence(*sceneLoadFence);

		// --- Shader work ---
		Pipeline::Attribute vtxPos;
		vtxPos.binding = 0;
		vtxPos.componentType = Pipeline::DataType::Float;
		vtxPos.numComponents = 3;
		vtxPos.offset = 0;
		vtxPos.stride = 3 * sizeof(float);

		Pipeline::Uniform vtxUniforms[2];
		// World matrix
		vtxUniforms[0].componentType = Pipeline::Float;
		vtxUniforms[0].numComponents = 16;
		vtxUniforms[0].count = 0;
		// View-Projection matrix
		vtxUniforms[1].componentType = Pipeline::Float;
		vtxUniforms[1].numComponents = 16;
		vtxUniforms[1].count = 0;

		Pipeline::PipielineDesc shaderDesc;
		shaderDesc.numAttributes = 1;
		shaderDesc.vtxAttributes = &vtxPos;
		shaderDesc.vtxUniforms.numUniforms = 2;
		shaderDesc.vtxUniforms.uniform = vtxUniforms;
		auto vtxCode = ShaderCodeFragment::loadFromFile("vertex.hlsl");
		vtxCode->collapse(shaderDesc.vtxCode);
		auto pxlCode = ShaderCodeFragment::loadFromFile("fragment.hlsl");
		pxlCode->collapse(shaderDesc.pxlCode);
		m_gBufferShader = m_gfxDevice->createPipeline(shaderDesc);

		// Dynamic shader reload
		auto vtxHook = vtxCode->onReload([&](ShaderCodeFragment& vtxFragment)
		{
			shaderDesc.vtxCode.clear();
			vtxFragment.collapse(shaderDesc.vtxCode);
			auto newShader = m_gfxDevice->createPipeline(shaderDesc);
			if (newShader)
				m_gBufferShader = newShader;
		});

		auto pxlHook = pxlCode->onReload([&](ShaderCodeFragment& pxlFragment)
		{
			shaderDesc.pxlCode.clear();
			pxlFragment.collapse(shaderDesc.pxlCode);
			auto newShader = m_gfxDevice->createPipeline(shaderDesc);
			if (newShader)
				m_gBufferShader = newShader;
		});

		sceneLoadFence->waitForValue(copyFenceValue);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update()
	{
		core::Time::get()->update();
		auto dt = core::Time::get()->frameTime();
		
		// Render scene
		auto& graphicsQueue = m_gfxDevice->commandQueue(0);

		// Render
		CommandPool* cmdPool = m_frameCmdPools[m_backBufferIndex];
		GpuBuffer* backBuffer = m_backBuffers[m_backBufferIndex];
		cmdPool->reset();
		m_frameCmdList->reset(*cmdPool);
		m_frameCmdList->resourceBarrier(m_backBuffers[m_backBufferIndex], CommandList::Barrier::Transition, CommandList::ResourceState::Present, CommandList::ResourceState::RenderTarget);
		m_frameCmdList->clearRenderTarget(m_swapChain->renderTarget(m_backBufferIndex), Vec4f(0.f, 1.f, 0.f, 1.f));

		m_frameCmdList->bindPipeline(m_gBufferShader);
		// Global uniforms
		m_frameCmdList->setConstants(0, sizeof(math::Mat44f), math::Mat44f::identity().data());
		m_frameCmdList->setConstants(1, sizeof(math::Mat44f), math::Mat44f::identity().data());
		m_frameCmdList->bindAttribute(0, 3 * sizeof(math::Vec3f), sizeof(math::Vec3f), m_sceneVertexBuffer);
		m_frameCmdList->bindIndexBuffer(3 * sizeof(uint16_t), CommandList::NdxBufferFormat::U16, m_sceneIndexBuffer);
		// Instance Uniforms
		m_frameCmdList->bindRenderTarget(m_swapChain->renderTarget(m_backBufferIndex));
		m_frameCmdList->setViewport(Vec2u::zero(), m_windowSize);
		m_frameCmdList->setScissor(Vec2u::zero(), m_windowSize);
		m_frameCmdList->drawIndexed(0, 3, 0);

		m_frameCmdList->resourceBarrier(m_backBuffers[m_backBufferIndex], CommandList::Barrier::Transition, CommandList::ResourceState::RenderTarget, CommandList::ResourceState::Present);
		m_frameCmdList->close();

		graphicsQueue.executeCommandList(*m_frameCmdList);
		m_frameFenceValues[m_backBufferIndex] = graphicsQueue.signalFence(*m_frameFence);

		// Present to screen
		m_swapChain->present();
		m_backBufferIndex = m_swapChain->getCurrentBackBuffer();
		m_frameFence->waitForValue(m_frameFenceValues[m_backBufferIndex]);

		return true;
	}

}	// namespace rev