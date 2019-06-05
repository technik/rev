//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "player.h"

#include <core/platform/cmdLineParser.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/fileSystem/file.h>
#include <core/time/time.h>
#include <game/scene/camera.h>
#include <game/scene/gltf/gltf.h>
#include <game/scene/sceneNode.h>
#include <game/scene/transform/transform.h>
#include <game/scene/transform/flyby.h>
#include <graphics/backend/device.h>
#include <graphics/backend/DirectX12/directX12Driver.h>
#include <graphics/backend/doubleBufferSwapChain.h>
#include <graphics/scene/renderGeom.h>
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
		swapChainInfo.pixelFormat.componentType = ScalarType::uint8;
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

		// Create depth buffer
		m_depthBuffer = m_gfxDevice->createDepthBuffer(m_windowSize);
		auto depth_heap = m_gfxDevice->createDescriptorHeap(1, DescriptorType::DepthStencil);
		m_depthBV = m_gfxDevice->createRenderTargetView(*depth_heap, nullptr, RenderTargetType::Depth, *m_depthBuffer);

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

		initRaytracing();

		createCamera();

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

	namespace {
		RenderGeom::Attribute readAttribute(const fx::gltf::Accessor& accessor, const fx::gltf::BufferView& bv, GpuBuffer* data)
		{
			RenderGeom::Attribute attribute;
			attribute.count = accessor.count;
			attribute.offset = accessor.byteOffset + bv.byteOffset;
			attribute.byteLenght = bv.byteLength - accessor.byteOffset;
			attribute.data = data;
			// Component type
			switch (accessor.componentType)
			{
			case fx::gltf::Accessor::ComponentType::UnsignedByte:
			{
				attribute.componentType = RenderGeom::VtxFormat::Storage::u8;
				break;
			}
			case fx::gltf::Accessor::ComponentType::UnsignedShort:
			{
				attribute.componentType = RenderGeom::VtxFormat::Storage::u16;
				break;
			}
			case fx::gltf::Accessor::ComponentType::UnsignedInt:
			{
				attribute.componentType = RenderGeom::VtxFormat::Storage::u32;
				break;
			}
			case fx::gltf::Accessor::ComponentType::Float:
			{
				attribute.componentType = RenderGeom::VtxFormat::Storage::Float32;
				break;
			}
			case fx::gltf::Accessor::ComponentType::None:
			{
				attribute.componentType = RenderGeom::VtxFormat::Storage::None;
				break;
			}
			default:
			{
				assert(false && "Component type not supported");
				break;
			}
			}
			// Element type
			switch (accessor.type)
			{
			case fx::gltf::Accessor::Type::Scalar:
				attribute.nComponents = 1;
				break;
			case fx::gltf::Accessor::Type::Vec2:
				attribute.nComponents = 2;
				break;
			case fx::gltf::Accessor::Type::Vec3:
				attribute.nComponents = 3;
				break;
			case fx::gltf::Accessor::Type::Vec4:
				attribute.nComponents = 4;
				break;
			case fx::gltf::Accessor::Type::Mat2:
				attribute.nComponents = 4;
				break;
			case fx::gltf::Accessor::Type::Mat3:
				attribute.nComponents = 9;
				break;
			case fx::gltf::Accessor::Type::Mat4:
				attribute.nComponents = 16;
				break;
			}
			// Stride
			attribute.stride = bv.byteStride ? bv.byteStride : attribute.elementSize();

			// Optional bounding box
			if (accessor.min.size() > 0 && accessor.max.size() > 0)
			{
				attribute.bounds = math::AABB(
					*reinterpret_cast<const math::Vec3f*>(accessor.min.data()),
					*reinterpret_cast<const math::Vec3f*>(accessor.max.data()));
			}
			else attribute.bounds.clear();

			return attribute;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::loadScene(const std::string& scene)
	{
		// Load gltf file
		std::filesystem::path scenePath(scene);
		auto assetFilename = core::FileSystem::get()->resolvePath(scenePath);
		if (assetFilename.empty())
		{
			std::cout << "Unable to locate asset: " << scene << "\n";
			return;
		}

		auto folder = assetFilename.parent_path();
		// Load gltf document
		fx::gltf::Document document;
		core::File sceneFile(assetFilename.generic_u8string());
		auto jsonText = sceneFile.bufferAsText();
		document = fx::gltf::detail::Create(
			nlohmann::json::parse(jsonText, nullptr, false),
			{ folder.generic_u8string(), {} });

		// Verify document is supported
		auto asset = document.asset;
		if (asset.version != "2.0") {
			std::cout << "Wrong format version. GLTF assets must be 2.0\n";
			return;
		}

		// Copy buffer into the GPU
		if (document.buffers.size() > 1)
		{
			std::cout << "Unsupported file. Only one buffer is supported per gltf file\n";
		}
		CommandList* copyCmdList = m_gfxDevice->createCommandList(CommandList::Copy, *m_copyCommandPool);
		auto bufferSize = document.buffers[0].byteLength;
		auto* stagingBuffer = m_gfxDevice->createCommitedResource(BufferType::Upload, bufferSize);
		m_sceneGpuBuffer = m_gfxDevice->createCommitedResource(BufferType::Resident, bufferSize);
		copyCmdList->uploadBufferContent(*m_sceneGpuBuffer, *stagingBuffer, bufferSize, document.buffers[0].data.data());
		// Immediately execute copy on a command queue
		copyCmdList->close();
		auto& copyQueue = m_gfxDevice->commandQueue(2);
		copyQueue.executeCommandList(*copyCmdList);
		Fence* sceneLoadFence = m_gfxDevice->createFence();
		auto copyFenceValue = copyQueue.signalFence(*sceneLoadFence);

		// Create Buffer views and attributes for the first mesh available
		auto& primitive = document.meshes[0].primitives[0];
		auto& indexAccessor = document.accessors[primitive.indices];
		auto& indexBv = document.bufferViews[indexAccessor.bufferView];
		RenderGeom::Attribute indexAttribute = readAttribute(indexAccessor, indexBv, m_sceneGpuBuffer);
		auto& positionAccessor = document.accessors[primitive.attributes.at("POSITION")];
		auto& positionBv = document.bufferViews[positionAccessor.bufferView];
		RenderGeom::Attribute positionAttribute = readAttribute(positionAccessor, positionBv, m_sceneGpuBuffer);

		m_geom = new RenderGeom(indexAttribute, positionAttribute, nullptr, nullptr, nullptr, nullptr, nullptr);

		// --- Shader work ---
		RootSignature::Desc rootSignatureDesc;
		rootSignatureDesc.addParam<math::Mat44f>(0); // World
		rootSignatureDesc.addParam<math::Mat44f>(4); // WorldViewProj
		m_rasterSignature = m_gfxDevice->createRootSignature(rootSignatureDesc);

		RasterPipeline::Attribute vtxPos;
		vtxPos.binding = 0;
		vtxPos.componentType = ScalarType::float32;
		vtxPos.numComponents = 3;
		vtxPos.offset = 0;
		vtxPos.stride = 3 * sizeof(float);

		RasterPipeline::Desc shaderDesc;
		shaderDesc.signature = m_rasterSignature;
		shaderDesc.numAttributes = 1;
		shaderDesc.vtxAttributes = &vtxPos;
		auto vtxCode = ShaderCodeFragment::loadFromFile("vertex.hlsl");
		vtxCode->collapse(shaderDesc.vtxCode);
		auto pxlCode = ShaderCodeFragment::loadFromFile("fragment.hlsl");
		pxlCode->collapse(shaderDesc.pxlCode);

		// Raster options
		RasterOptions rasterOptions;
		rasterOptions.cullBack = true;
		rasterOptions.depthTest = ZTest::Gequal;

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
	void Player::initRaytracing()
	{
		// Create global root signature
		Microsoft::WRL::ComPtr<ID3DBlob> blob;
		Microsoft::WRL::ComPtr<ID3DBlob> error;

		//ThrowIfFailed(D3D12SerializeRootSignature(&desc, D3D_ROOT_SIGNATURE_VERSION_1, &blob, &error), error ? static_cast<wchar_t*>(error->GetBufferPointer()) : nullptr);
		//ThrowIfFailed(device->CreateRootSignature(1, blob->GetBufferPointer(), blob->GetBufferSize(), IID_PPV_ARGS(&(*rootSig))));		
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera() {

		// Create fliby camera
		m_camNode = new SceneNode("Flyby cam");
		m_camNode->addComponent<FlyBy>(2.f, 1.f);
		m_camNode->addComponent<Transform>()->xForm.position() = math::Vec3f{ 0.0f, 0.f, 9.f };
		auto camComponent = m_camNode->addComponent<game::Camera>(math::radians(45.f), 0.01f, 100.f);
		m_renderCam = &*camComponent->cam();

		m_camNode->init();
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update()
	{
		core::Time::get()->update();
		auto dt = core::Time::get()->frameTime();

		// Update nodes
		m_camNode->update(dt);
		
		// Render scene
		auto& graphicsQueue = m_gfxDevice->commandQueue(0);

		// Render
		CommandPool* cmdPool = m_frameCmdPools[m_backBufferIndex];
		GpuBuffer* backBuffer = m_backBuffers[m_backBufferIndex];
		cmdPool->reset();
		m_frameCmdList->reset(*cmdPool);
		m_frameCmdList->resourceBarrier(m_backBuffers[m_backBufferIndex], CommandList::Barrier::Transition, CommandList::ResourceState::Present, CommandList::ResourceState::RenderTarget);
		m_frameCmdList->clearRenderTarget(m_swapChain->renderTarget(m_backBufferIndex), Vec4f(0.f, 1.f, 0.f, 1.f));
		m_frameCmdList->clearDepth(m_depthBV, 0.f);

		m_frameCmdList->bindRootSignature(m_rasterSignature);
		m_frameCmdList->bindPipeline(m_gBufferShader);
		m_frameCmdList->bindRenderTarget(m_swapChain->renderTarget(m_backBufferIndex), m_depthBV);
		m_frameCmdList->setViewport(Vec2u::zero(), m_windowSize);
		m_frameCmdList->setScissor(Vec2u::zero(), m_windowSize);
		// Global uniforms
		// Compute view projection matrix
		float aspectRatio = float(m_windowSize.x()) / m_windowSize.y();
		math::Mat44f viewProj = m_renderCam->viewProj(aspectRatio).transpose();
		m_frameCmdList->setConstants(0, sizeof(math::Mat44f), viewProj.data());
		m_frameCmdList->bindAttribute(0, m_geom->vertices().byteLenght, m_geom->vertices().stride, m_geom->vertices().data, m_geom->vertices().offset);
		m_frameCmdList->bindIndexBuffer(m_geom->indices().byteLenght, CommandList::NdxBufferFormat::U16, m_geom->indices().data);
		// Instance Uniforms
		m_frameCmdList->setConstants(1, sizeof(math::Mat44f), math::Mat44f::identity().data());

		m_frameCmdList->drawIndexed(0, m_geom->indices().count, 0);

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