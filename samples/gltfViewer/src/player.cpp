//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "player.h"

#include <core/platform/cmdLineParser.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <core/time/time.h>
#include <graphics/backend/device.h>
#include <graphics/backend/DirectX12/directX12Driver.h>
#include <graphics/backend/doubleBufferSwapChain.h>
#include <graphics/shaders/shaderCodeFragment.h>
#include <math/algebra/vector.h>
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
		auto* vtxBuffer = m_gfxDevice->createCommitedResource(Device::BufferType::Resident, Device::ResourceFlags::None, 3 * sizeof(rev::math::Vec3f));

		rev::math::Vec3f vtx[3] = {
			rev::math::Vec3f(0.5f,0.1f,0.5f),
			rev::math::Vec3f(0.f,-1.f,0.f),
			rev::math::Vec3f(-0.5f,0.f,0.f)
		};
		copyCmdList->uploadBufferContent(*vtxBuffer, *stagingBuffer, 3 * sizeof(rev::math::Vec3f), vtx);

		// Create buffers for index data
		auto* indexStagingBuffer = m_gfxDevice->createCommitedResource(Device::BufferType::Upload, Device::ResourceFlags::None, 3 * sizeof(uint16_t));
		auto* indexBuffer = m_gfxDevice->createCommitedResource(Device::BufferType::Resident, Device::ResourceFlags::None, 3 * sizeof(uint16_t));

		uint16_t indices[3] = { 0, 1, 2 };
		copyCmdList->uploadBufferContent(*indexBuffer, *indexStagingBuffer, 3 * sizeof(uint16_t), indices);

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

		Pipeline::Uniform vtxUniforms[1];
		vtxUniforms[0].componentType = Pipeline::Float;
		vtxUniforms[0].numComponents = 16;
		vtxUniforms[0].count = 1;

		Pipeline::PipielineDesc shaderDesc;
		shaderDesc.numAttributes = 1;
		shaderDesc.vtxAttributes = &vtxPos;
		shaderDesc.vtxUniforms.numUniforms = 1;
		shaderDesc.vtxUniforms.uniform = vtxUniforms;
		auto vtxCode = ShaderCodeFragment::loadFromFile("vertex.hlsl");
		vtxCode->collapse(shaderDesc.vtxCode);
		auto pxlCode = ShaderCodeFragment::loadFromFile("fragment.hlsl");
		pxlCode->collapse(shaderDesc.pxlCode);
		Pipeline* triShader = m_gfxDevice->createPipeline(shaderDesc);

		// Dynamic shader reload
		auto vtxHook = vtxCode->onReload([&](ShaderCodeFragment& vtxFragment)
		{
			shaderDesc.vtxCode.clear();
			vtxFragment.collapse(shaderDesc.vtxCode);
			auto newShader = m_gfxDevice->createPipeline(shaderDesc);
			if (newShader)
				triShader = newShader;
		});

		auto pxlHook = pxlCode->onReload([&](ShaderCodeFragment& pxlFragment)
		{
			shaderDesc.pxlCode.clear();
			pxlFragment.collapse(shaderDesc.pxlCode);
			auto newShader = m_gfxDevice->createPipeline(shaderDesc);
			if (newShader)
				triShader = newShader;
		});

		sceneLoadFence->waitForValue(copyFenceValue);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::updateSceneBBox()
	{
		// Compute scene bbox
		/*m_globalBBox.clear();
		m_gltfRoot->traverseSubtree([&](SceneNode& node){
			if(auto renderer = node.component<game::MeshRenderer>())
			{
				auto localbbox = renderer->renderObj().mesh->m_bbox;
				auto transform = node.component<Transform>();
				auto wsBbox = transform->absoluteXForm() * localbbox;
				m_globalBBox.add(wsBbox);
			}
		});

		// Re-center scene
		auto xForm = m_gltfRoot->component<Transform>();
		xForm->xForm.position() = -m_globalBBox.origin();*/
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera() {
		
		// Create fliby camera
		/*auto cameraNode = mGameScene.root()->createChild("Flyby cam");
		m_flyby = cameraNode->addComponent<FlyBy>(2.f, 1.f);
		cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { 0.0f, 0.f, 9.f };
		//cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { -2.5f, 1.f, 3.f };
		//cameraNode->component<Transform>()->xForm.rotate(Quatf({0.f,1.f,0.f}, -0.5f*Constants<float>::halfPi));
		auto camComponent = cameraNode->addComponent<game::Camera>(math::Pi/5, 0.01f, 100.f);
		mFlybyCam = &*camComponent->cam();
		
		// Create orbit camera
		cameraNode = mGameScene.root()->createChild("Orbit cam");
		m_orbit = cameraNode->addComponent<Orbit>(Vec2f{2.f, 1.f});
		cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { -2.5f, 1.f, 3.f };
		cameraNode->component<Transform>()->xForm.rotate(Quatf({0.f,1.f,0.f}, -0.5f*Constants<float>::halfPi));
		camComponent = cameraNode->addComponent<game::Camera>(math::Pi/4, 0.01f, 100.f);
		mOrbitCam = &*camComponent->cam();*/
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createFloor() {

		/*auto floorNode = mGameScene.root()->createChild("floor");
		auto sceneSize = m_globalBBox.size();
		auto floorXForm = floorNode->addComponent<Transform>();
		floorXForm->xForm.rotate(Quatf({1.f,0.f,0.f}, -math::Constants<float>::halfPi));
		floorXForm->xForm.position() = m_globalBBox.origin();
		floorXForm->xForm.position().y() = m_globalBBox.min().y();

		const float floorScale = 4.f;
		auto floorMesh = std::make_shared<gfx::RenderGeom>(RenderGeom::quad(floorScale * Vec2f(sceneSize.x(), sceneSize.z())));

		// Create default material
		auto pbrEffect = std::make_shared<Effect>("shaders/metal-rough.fx");
		auto defaultMaterial = std::make_shared<Material>(pbrEffect);

		gfx::TextureSampler::Descriptor samplerDesc;
		samplerDesc.wrapS = gfx::TextureSampler::Wrap::Clamp;
		samplerDesc.wrapT = gfx::TextureSampler::Wrap::Clamp;
		auto sampler = m_gfx.createTextureSampler(samplerDesc);
		auto envBRDF = load2dTextureFromFile(m_gfx, sampler, "shaders/ibl_brdf.hdr", false, 1);
		defaultMaterial->addTexture("uEnvBRDF", envBRDF);

		// Create mesh renderer component
		auto renderable = std::make_shared<gfx::RenderMesh>();
		renderable->mPrimitives.push_back({floorMesh, defaultMaterial});
		m_floorGeom = std::make_shared<gfx::RenderObj>(renderable);
		m_floorGeom->visible = false;
		mGraphicsScene.renderables().push_back(m_floorGeom);

		floorNode->addComponent<game::MeshRenderer>(m_floorGeom);*/
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update()
	{
		core::Time::get()->update();
		auto dt = core::Time::get()->frameTime();
		
		// Render scene
		auto& graphicsQueue = m_gfxDevice->commandQueue(0);

		// Present to screen
		//m_gfx.renderQueue().present();
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::updateUI(float dt)
	{
		/*gui::startFrame(m_windowSize);

		if(ImGui::Begin("Player options"))
		{
			ImGui::InputFloat("Camera speed", &m_flyby->speed());
			ImGui::Checkbox("Floor", &m_floorGeom->visible);
			bool fwdRender = m_renderPath == RenderPath::Forward;
			ImGui::Checkbox("Forward", &fwdRender);
			if(fwdRender)
				m_renderPath = RenderPath::Forward;
			else
				m_renderPath = RenderPath::Deferred;
		}
		ImGui::End();

		if(ImGui::Begin("Render options"))
		{
			ImGui::Checkbox("IBL Shadows", &m_bgOptions.shadows);
			if(m_bgOptions.shadows)
			{
				gui::slider("Shadow elevation", m_bgOptions.elevation, 0.f, math::Constants<float>::halfPi);
				gui::slider("Shadow rotation", m_bgOptions.rotation, 0.f, math::Constants<float>::twoPi);

				gui::slider("Shadow bias", mForwardRenderer.shadowBias(), -0.1f, 0.1f);
			}
			m_envLight->castShadows = m_bgOptions.shadows;
		}
		ImGui::End();
		mForwardRenderer.drawDebugUI();
		
		auto elevation = Quatf(normalize(Vec3f(1.f, 0.f, 0.f)), -m_bgOptions.elevation);
		auto rotation = Quatf(normalize(Vec3f(0.f, 1.f, 0.f)), m_bgOptions.rotation);

		AffineTransform lightXform = AffineTransform::identity();
		lightXform.setRotation(rotation * elevation);
		m_envLight->worldMatrix = lightXform;

		gui::finishFrame(dt);*/
	}

}	// namespace rev