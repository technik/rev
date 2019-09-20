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
#include <graphics/scene/renderMesh.h>
#include <graphics/shaders/shaderCodeFragment.h>
#include <math/algebra/vector.h>
#include <math/algebra/matrix.h>
#include <iostream>

#include <graphics/renderGraph/renderGraph.h>

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
		swapChainInfo.pixelFormat.components = 4;
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

		// Concept code to replace the simple G-Buffer pass below
		//*
		graph.addPass("G-Pass",
			[this](RenderGraph::PassBuilder& pass)
			{
				//auto z = graph.createDepthRT(m_windowSize);
				auto z = graph.importDepthRT(m_windowSize, *m_depthBuffer, *m_depthBV, CommandList::ResourceState::RenderTarget);
				pass.clear(z, 0.f);
				pass.write(z);

				auto backBuffer = m_backBuffers[m_backBufferIndex];
				auto backBufferRT = m_swapChain->renderTarget(m_backBufferIndex);
				auto gBuffer = graph.importRT(m_windowSize, *backBuffer, *backBufferRT, CommandList::ResourceState::Present);
				pass.clear(gBuffer, math::Vec4f::zero());
				pass.write(0, gBuffer); // 0 is the binding spot
			},
			[this](CommandList& cmdList){
				cmdList.bindRootSignature(m_rasterSignature);
				cmdList.bindPipeline(m_gBufferShader);

				// Global uniforms
				float aspectRatio = float(m_windowSize.x()) / m_windowSize.y();
				math::Mat44f projMatrix = m_renderCam->projection(aspectRatio);
				math::Mat44f view = m_renderCam->view();
				math::Mat44f viewProj = (projMatrix * view);

				// Attributes
				for (auto& mesh : m_geom)
				{
					math::Mat44f worldViewProj = viewProj;

					// Instance Uniforms
					cmdList.setConstants(0, sizeof(math::Mat44f), worldViewProj.data());
					cmdList.setConstants(1, sizeof(math::Mat44f), math::Mat44f::identity().data());

					for (auto& [primitive, material] : mesh->mPrimitives)
					{
						cmdList.bindAttributes(primitive->numAttributes(), primitive->attributes());
						cmdList.bindIndexBuffer(primitive->indices().byteLenght, CommandList::NdxBufferFormat::U16, primitive->indices().data, primitive->indices().offset);

						cmdList.drawIndexed(0, primitive->indices().count, 0);
					}
				}
			});
		//*/

		// Create command list for copying data
		m_copyCommandPool = m_gfxDevice->createCommandPool(CommandList::Copy);
		
		loadScene(options.scene);

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
		VertexAttribute readAttribute(const fx::gltf::Document& document, const fx::gltf::Accessor& accessor, GpuBuffer* data)
		{
			auto& bv = document.bufferViews[accessor.bufferView];
			VertexAttribute attribute;
			attribute.count = accessor.count;
			attribute.offset = accessor.byteOffset + bv.byteOffset;
			attribute.byteLenght = bv.byteLength - accessor.byteOffset;
			attribute.data = data;
			// Component type
			switch (accessor.componentType)
			{
			case fx::gltf::Accessor::ComponentType::UnsignedByte:
			{
				attribute.format.componentType = ScalarType::uint8;
				break;
			}
			case fx::gltf::Accessor::ComponentType::UnsignedShort:
			{
				attribute.format.componentType = ScalarType::uint16;
				break;
			}
			case fx::gltf::Accessor::ComponentType::UnsignedInt:
			{
				attribute.format.componentType = ScalarType::uint32;
				break;
			}
			case fx::gltf::Accessor::ComponentType::Float:
			{
				attribute.format.componentType = ScalarType::float32;
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
				attribute.format.components = 1;
				break;
			case fx::gltf::Accessor::Type::Vec2:
				attribute.format.components = 2;
				break;
			case fx::gltf::Accessor::Type::Vec3:
				attribute.format.components = 3;
				break;
			case fx::gltf::Accessor::Type::Vec4:
				attribute.format.components = 4;
				break;
			case fx::gltf::Accessor::Type::Mat2:
				attribute.format.components = 4;
				break;
			case fx::gltf::Accessor::Type::Mat3:
				attribute.format.components = 9;
				break;
			case fx::gltf::Accessor::Type::Mat4:
				attribute.format.components = 16;
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

		// Geometry vertex format
		RenderGeom::VtxFormat vtxFormat(RenderGeom::VtxFormat::Storage::Float32, RenderGeom::VtxFormat::Storage::Float32);

		// Load meshes
		m_geom.reserve(document.meshes.size());
		for (auto& mesh : document.meshes)
		{
			auto dstMesh = std::make_shared<gfx::RenderMesh>();

			dstMesh->mPrimitives.reserve(mesh.primitives.size());

			for(auto& primitive : mesh.primitives)
			{
				auto& indexAccessor = document.accessors[primitive.indices];
				auto& positionAccessor = document.accessors[primitive.attributes.at("POSITION")];
				auto& normalAccessor = document.accessors[primitive.attributes.at("NORMAL")];

				VertexAttribute indexAttribute = readAttribute(document, indexAccessor, m_sceneGpuBuffer);
				VertexAttribute vtxAttributes[2];
				vtxAttributes[0] = readAttribute(document, positionAccessor, m_sceneGpuBuffer);
				vtxAttributes[1] = readAttribute(document, normalAccessor, m_sceneGpuBuffer);
				
				dstMesh->mPrimitives.emplace_back(new RenderGeom(vtxFormat, indexAttribute, vtxAttributes, 2));
			}

			dstMesh->updateBBox();
			m_geom.push_back(dstMesh);
		}

		// --- Shader work ---
		RootSignature::Desc rootSignatureDesc;
		rootSignatureDesc.addParam<math::Mat44f>(0); // World
		rootSignatureDesc.addParam<math::Mat44f>(4); // WorldViewProj
		m_rasterSignature = m_gfxDevice->createRootSignature(rootSignatureDesc);

		RasterPipeline::Attribute attributes[2];
		// Position
		attributes[0].binding = 0;
		attributes[0].format.componentType = ScalarType::float32;
		attributes[0].format.components = 3;
		attributes[0].offset = vtxAttributes[0].offset;
		attributes[0].stride = vtxAttributes[0].stride;
		attributes[0].name = "position";
		// Normal
		attributes[1].binding = 0;
		attributes[1].format.componentType = ScalarType::float32;
		attributes[1].format.components = 3;
		attributes[1].offset = vtxAttributes[1].offset;
		attributes[1].stride = vtxAttributes[1].stride;
		attributes[1].name = "normal";

		RasterPipeline::Desc shaderDesc;
		shaderDesc.signature = m_rasterSignature;
		shaderDesc.numAttributes = 2;
		shaderDesc.vtxAttributes = attributes;
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
	void Player::createCamera() {

		// Create fliby camera
		m_camNode = new SceneNode("Flyby cam");
		m_camNode->addComponent<FlyBy>(2.f, 1.f);
		m_camNode->addComponent<Transform>()->xForm.position() = math::Vec3f{ 0.0f, 0.f, 5.f };
		auto camComponent = m_camNode->addComponent<game::Camera>(math::radians(45.f), 0.01f, 10.f);
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


		// Concept code for how the render graph API would be used
		/*
		RenderGraph graph;
		// GBuffer pass
		int gBuffer; // Actually a rendergraph::RT resource, so it's type safe
		auto depthBuffer = graph.addDB(f32);

		graph.passCallback("G-Pass", 
			[&]() {
				gBuffer = pass.create(Vec4f::zero());
				pass.clear(depthBuffer);
				pass.write(depthBuffer);
			},
			[](CommandList& cmdList) {
				cmdList.bindRootSignature();
				cmdList.bindPipeline();

				cmdList.bindAttributes();
				cmdList.drawIndexed();
		});

		// AO Pass
		auto aoBuffer = graph.addFilterPass(2, { depthBuffer, gBuffer }, "AO.hlsl");

		// Color passes
		auto colorBuffer = graph.addRT(3, f32);
		// Background pass
		graph.clearPass(colorBuffer, Vec4f::zero());
		graph.addFilterPass(2, { m_depthBuffer, m_gBuffer }, colorBuffer, "background.hlsl"); // Here, color buffer is taken by reference, and modified because the pass writes to it, true at the end forces a clear
		// IBL Pass
		graph.addFilterPass(3, { m_depthBuffer, m_gBuffer, aoBuffer }, colorBuffer, "IBL.hlsl");
		// Transparent forward pass
		graph.passCallback("Transparent forward", 
			[&]() {
				pass.read(depthBuffer);
				pass.write(colorBuffer)
			},
			[](CommandList& cmdList) {
				cmdList.bindRootSignature();
				cmdList.bindPipeline();

				cmdList.bindAttributes();
				cmdList.drawIndexed();
		});
		// HDR Pass
		auto backBuffer = graph.importRT(m_backBufferIndex, ResourceState::RT);
		graph.addFilterPass(colorBuffer, backBuffer, "hdr.hlsl");
		//*/

		// Render
		CommandPool* cmdPool = m_frameCmdPools[m_backBufferIndex];
		GpuBuffer* backBuffer = m_backBuffers[m_backBufferIndex];
		cmdPool->reset();
		m_frameCmdList->reset(*cmdPool);
		m_frameCmdList->resourceBarrier(m_backBuffers[m_backBufferIndex], CommandList::Barrier::Transition, CommandList::ResourceState::Present, CommandList::ResourceState::RenderTarget);

		//m_frameCmdList->bindRenderTarget(m_swapChain->renderTarget(m_backBufferIndex), m_depthBV);
		//m_frameCmdList->setViewport(Vec2u::zero(), m_windowSize);
		//m_frameCmdList->setScissor(Vec2u::zero(), m_windowSize);

		graph.compile(*m_gfxDevice);
		graph.record(*m_frameCmdList);

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