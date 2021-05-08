//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "player.h"
#include <math/algebra/vector.h>
#include <core/platform/fileSystem/file.h>
#include <core/platform/fileSystem/FolderWatcher.h>
#include <core/platform/cmdLineParser.h>
#include <core/platform/osHandler.h>
#include <core/tools/log.h>
#include <input/pointingInput.h>

#include <imgui/imgui.h>
#include <imgui/backends/imgui_impl_vulkan.h>
#include <imgui/backends/imgui_impl_win32.h>

#include <game/scene/camera.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/gltf/gltfLoader.h>
#include <game/scene/transform/flyby.h>
#include <game/scene/transform/orbit.h>
#include <game/scene/transform/transform.h>

using namespace rev::math;
using namespace rev::gfx;
using namespace rev::game;

// Imgui windows handler
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace rev {
	//------------------------------------------------------------------------------------------------------------------
	void Player::CommandLineOptions::registerOptions(core::CmdLineParser& args)
	{
		args.addOption("env", &environment);
		args.addOption("scene", &scene);
		args.addOption("fov", &fov);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::getCommandLineOptions(core::CmdLineParser& args)
	{
		m_options.registerOptions(args);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init()
	{
		//
		// Ideal interface for render graph
		// 
		// G-Buffer pass -> Normals, PBR, Z, Motion vectors
		// [Normals, PBR, Z] = GBufferPass();
		// Direct light
		//	- Light sample (Normals, PBR, Z) -> Direct Light Reservoir [DLR]
		//	- Temporal ReStir (DLR-1, Normals, Z) -> DLR
		//	- Spatial ReStir (DLR, Normals, Z) -> DLR
		//	- Shadow trace (Z, DLR) -> Direct Light
		// Indirect light 
		// Direct diffuse denoise
		// Direct spec denoise
		// Indirect denoise
		// Post process
		// 
		// UI
		// UIPass(GBuffer, SwapChain);

		// Start with
		// 
		// GBufferNormals = createColorBuffer("name", size, format);
		// Z = createDepthBuffer("name", size, format);
		// 
		// objs = CullInstances()
		// GBufferPass(objs)


		auto device = renderContext().device();

		// Create semaphores
		m_imageAvailableSemaphore = device.createSemaphore({});

		// Init descriptor sets
		const uint32_t numDescriptors = 2 * 2; // 2 per frame * 2 frames
		vk::DescriptorPoolSize poolSize(vk::DescriptorType::eStorageBuffer, numDescriptors);

		auto poolInfo = vk::DescriptorPoolCreateInfo({}, numDescriptors, poolSize);
		m_descPool = device.createDescriptorPool(poolInfo);

		vk::DescriptorSetLayoutBinding matricesBinding(0, vk::DescriptorType::eStorageBuffer, 1, vk::ShaderStageFlagBits::eVertex);

		vk::DescriptorSetLayoutCreateInfo setLayoutInfo({}, matricesBinding);
		m_frameDescLayout = device.createDescriptorSetLayout(setLayoutInfo);
		vk::DescriptorSetLayout setLayouts[2] = { m_frameDescLayout, m_frameDescLayout };
		
		vk::DescriptorSetAllocateInfo setInfo(m_descPool, 2, setLayouts);
		m_frameDescs = device.allocateDescriptorSets(setInfo);

		// UI pipeline layout
		vk::PushConstantRange camerasPushRange(
			vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
			0, sizeof(CameraPushConstants));

		vk::PipelineLayoutCreateInfo layoutInfo({},
			1, &m_frameDescLayout, // Descriptor sets
			1, &camerasPushRange); // Push constants

		m_gbufferPipelineLayout = device.createPipelineLayout(layoutInfo);

		// UI Render pass
		m_uiPass = renderContext().createRenderPass({
			{ renderContext().swapchainFormat(), vk::ImageLayout::eGeneral, vk::ImageLayout::eGeneral },
			{ vk::Format::eD32Sfloat, vk::ImageLayout::eGeneral, vk::ImageLayout::eGeneral }
			});

		createFrameBuffers();

		m_gBufferPipeline = std::make_unique<gfx::RasterPipeline>(
			device,
			m_gbufferPipelineLayout,
			m_uiPass,
			"../shaders/gbuffer.vert.spv",
			"../shaders/gbuffer.frag.spv",
			true);
		m_shaderWatcher = std::make_unique<core::FolderWatcher>(core::FolderWatcher::path("../shaders"));
		m_shaderWatcher->listen([this](auto paths) { m_gBufferPipeline->invalidate(); });

		// Init ImGui
		initImGui();

		m_sceneRoot = std::make_shared<SceneNode>("scene root");
		loadScene(m_options.scene);

		// Create camera
		createCamera();
		m_sceneRoot->init();

		m_cameraPushC.ambiendColor = Vec4f(0.2f, 0.2f, 0.4f);
		m_cameraPushC.lightColor = Vec4f(1.f, 1.f, 1.f);
		m_cameraPushC.lightDir = normalize(Vec4f(1.f, 1.f, 1.f, 0.f));

		/*
		// Default scene light
		{
			m_envLight = std::make_shared<gfx::DirectionalLight>();
			m_envLight->castShadows = true;
			AffineTransform lightXform = AffineTransform::identity();
			//lightXform.setRotation(Quatf(normalize(Vec3f(1.f, 0.f, 0.f)), Constants<float>::halfPi*0.4));
			m_envLight->worldMatrix = lightXform;
			m_envLight->color = 4.f*Vec3f::ones();
			mGraphicsScene.addLight(m_envLight);
		}

		// Load sky
		if(!m_options.environment.empty())
		{
			auto probe = std::make_shared<EnvironmentProbe>(gfxDevice(), m_options.environment);
			if(probe->numLevels())
				mGraphicsScene.setEnvironment(probe);
		}

		createFloor();
		*/
		return true;
	}
	//------------------------------------------------------------------------------------------------------------------
	void Player::end()
	{
		destroyFrameBuffers();

		auto device = renderContext().device();
		m_gBufferPipeline.reset();
		device.destroyPipelineLayout(m_gbufferPipelineLayout);
		device.destroyRenderPass(m_uiPass);
		device.destroySemaphore(m_imageAvailableSemaphore);
	}

#ifdef _WIN32
	//------------------------------------------------------------------------------------------------------------------
	void Player::onResize()
	{
		renderContext().resizeSwapchain(windowSize());

		createFrameBuffers();
	}
#endif // _WIN32

	//------------------------------------------------------------------------------------------------------------------
	void Player::loadScene(const std::string& scene)
	{
		if (scene.empty()) // Early out
			return;

		// TODO:
		// Preload metadata and scene definition
		// Allocate memory in the renderer for meshes (and maybe textures)
		// Load scene (meshes, mats, etc)
		GltfLoader gltfLoader(renderContext());
		auto loadRes = gltfLoader.load(scene, m_rasterData);
		m_sceneInstances = loadRes.meshInstances;

		// Load geometry data and consolidate it into a small set of continuous buffers

		const size_t numVertices = 3;
		const size_t numIndices = 3;
		const Vec3f vtxPos[numVertices] = {
			{0.0f, -0.5f, -1.f},
			{-0.5f, 0.5f, -1.f},
			{0.5f, 0.5f, -1.f}
		};
		const Vec3f vtxColors[numVertices] = {
			{1.0f, 0.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 0.0f, 1.0f}
		};
		const uint32_t indices[numIndices] = {
			0, 1, 2
		};

		// Allocate buffers
		auto& alloc = renderContext().allocator();
		m_vtxPosBuffer = alloc.createGpuBuffer(sizeof(vtxPos), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, renderContext().graphicsQueueFamily());
		m_vtxClrBuffer = alloc.createGpuBuffer(sizeof(vtxColors), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer, renderContext().graphicsQueueFamily());
		m_indexBuffer = alloc.createGpuBuffer(sizeof(indices), vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer, renderContext().graphicsQueueFamily());

		// Copy data to the GPU
		// Allocate enough room to stream all data at once
		alloc.reserveStreamingBuffer(sizeof(vtxColors) + sizeof(vtxPos) + sizeof(indices));
		alloc.asyncTransfer(*m_vtxPosBuffer, vtxPos, numVertices);
		alloc.asyncTransfer(*m_vtxClrBuffer, vtxColors, numVertices);
		alloc.asyncTransfer(*m_indexBuffer, indices, numIndices);

		// Allocate matrix buffers
		m_mtxBuffers.resize(2);
		m_mtxBuffers[0] = alloc.createBufferForMapping(sizeof(math::Mat44f) * m_sceneInstances.numInstances(), vk::BufferUsageFlagBits::eStorageBuffer, renderContext().graphicsQueueFamily());
		m_mtxBuffers[1] = alloc.createBufferForMapping(sizeof(math::Mat44f) * m_sceneInstances.numInstances(), vk::BufferUsageFlagBits::eStorageBuffer, renderContext().graphicsQueueFamily());

		// Update descriptor sets
		vk::WriteDescriptorSet writeInfo;
		writeInfo.dstSet = m_frameDescs[0];
		writeInfo.dstBinding = 0;
		writeInfo.dstArrayElement = 0;
		writeInfo.descriptorType = vk::DescriptorType::eStorageBuffer;
		writeInfo.descriptorCount = 1;
		vk::DescriptorBufferInfo writeBufferInfo(m_mtxBuffers[0]->buffer(), 0, m_mtxBuffers[0]->size());
		writeInfo.pBufferInfo = &writeBufferInfo;
		renderContext().device().updateDescriptorSets(writeInfo, {});
		writeInfo.dstSet = m_frameDescs[1];
		writeBufferInfo.buffer = m_mtxBuffers[1]->buffer();
		renderContext().device().updateDescriptorSets(writeInfo, {});

		m_sceneRoot->addChild(loadRes.rootNode);
		// Update scene representation in GUI
		// Optimize/Convert buffers to runtime formats
		// Instantiate nodes with render components
		// Load animations, if any
		// Load lights
		// Load cameras
		m_sceneLoadStreamToken = m_rasterData.closeAndSubmit(renderContext(), alloc);

		//m_gltfRoot = std::make_shared<SceneNode>("gltf scene parent");
		//m_gltfRoot->addComponent<Transform>();
		//mGameScene.root()->addChild(m_gltfRoot);

		//std::vector<std::shared_ptr<Animation>> animations;
		//std::vector<std::shared_ptr<SceneNode>> animNodes;
		//GltfLoader gltfLoader(gfxDevice());
		//gltfLoader.load(*m_gltfRoot, scene, mGraphicsScene, animNodes, animations);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::updateSceneBBox()
	{
		// Compute scene bbox
		m_globalBBox.clear();
		/*m_gltfRoot->traverseSubtree([&](SceneNode& node){
			if(auto renderer = node.component<game::MeshRenderer>())
			{
				auto localbbox = renderer->renderObj().mesh->m_bbox;
				auto transform = node.component<Transform>();
				auto wsBbox = transform->absoluteXForm().matrix() * localbbox;
				m_globalBBox.add(wsBbox);
			}
		});*/

		// Re-center scene
		//auto xForm = m_gltfRoot->component<Transform>();
		//xForm->xForm.position() = -m_globalBBox.center();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createFrameBuffers()
	{
		auto windowSize = renderContext().windowSize();
		auto& alloc = renderContext().allocator();

		m_gBufferNormals = alloc.createImageBuffer("normals", windowSize, vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment, renderContext().graphicsQueueFamily());
		m_gBufferPBR = alloc.createImageBuffer("PBR", windowSize, vk::Format::eR32G32B32A32Sfloat, vk::ImageUsageFlagBits::eColorAttachment, renderContext().graphicsQueueFamily());
		m_gBufferZ = alloc.createDepthBuffer(
			"Depth",
			windowSize,
			vk::Format::eD32Sfloat,
			vk::ImageUsageFlagBits::eDepthStencilAttachment | vk::ImageUsageFlagBits::eTransferDst,
			renderContext().graphicsQueueFamily());

		// Transition new images to general layout
		renderContext().transitionImageLayout(m_gBufferZ->image(), m_gBufferZ->format(), vk::ImageLayout::eUndefined, vk::ImageLayout::eGeneral, true);

		// Actual frame buffer creation
		m_swapchainFrameBuffers.clear();

		// Create frame buffers for each image in the swapchain
		m_swapchainFrameBuffers.reserve(renderContext().nSwapChainImages());
		for (size_t i = 0; i < renderContext().nSwapChainImages(); ++i) {
			std::vector<vk::ImageView> imageViews = { renderContext().swapchainImageView(i), m_gBufferZ->view() };
			auto fbInfo = vk::FramebufferCreateInfo({},
				m_uiPass,
				imageViews,
				(uint32_t)windowSize.x(), (uint32_t)windowSize.y(), 1);
			m_swapchainFrameBuffers.push_back(renderContext().device().createFramebuffer(fbInfo));
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::destroyFrameBuffers()
	{
		m_swapchainFrameBuffers.clear();

		m_gBufferNormals = nullptr;
		m_gBufferPBR = nullptr;
		m_gBufferZ = nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera()
	{	
		// Create flyby camera
		auto cameraNode = m_sceneRoot->createChild("Flyby cam");
		m_flyby = cameraNode->addComponent<FlyBy>(2.f, 1.f);
		cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { 0.0f, 0.f, 9.f };
		auto camComponent = cameraNode->addComponent<game::Camera>(math::Pi/5, 0.01f, 5000.f);
		mFlybyCam = &*camComponent->cam();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createFloor()
	{
		/*auto floorNode = mGameScene.root()->createChild("floor");
		auto sceneSize = m_globalBBox.size();
		auto floorXForm = floorNode->addComponent<Transform>();
		floorXForm->xForm.rotate(Quatf({1.f,0.f,0.f}, -math::Constants<float>::halfPi));
		floorXForm->xForm.position() = m_globalBBox.center();
		floorXForm->xForm.position().y() = m_globalBBox.min().y();

		const float floorScale = 4.f;
		auto floorMesh = std::make_shared<gfx::RenderGeom>(RenderGeom::quad(floorScale * Vec2f(sceneSize.x(), sceneSize.z())));

		// Create default material
		Material::Descriptor pbrMatDescriptor;
		pbrMatDescriptor.effect = std::make_shared<Effect>("shaders/metal-rough.fx");
		auto defaultMaterial = std::make_shared<Material>(pbrMatDescriptor);

		// Create mesh renderer component
		auto renderable = std::make_shared<gfx::RenderMesh>();
		renderable->mPrimitives.push_back({floorMesh, defaultMaterial});
		m_floorGeom = std::make_shared<gfx::RenderObj>(renderable);
		m_floorGeom->visible = false;
		mGraphicsScene.renderables().push_back(m_floorGeom);

		floorNode->addComponent<game::MeshRenderer>(m_floorGeom);*/
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::updateLogic(TimeDelta dt)
	{
		m_shaderWatcher->update();
		// TODO: Play animations
		m_sceneRoot->update(dt.count());
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::render(TimeDelta dt)
	{
		auto cmd = renderContext().getNewRenderCmdBuffer();

		// Record frame
		cmd.begin(vk::CommandBufferBeginInfo(vk::CommandBufferUsageFlagBits::eOneTimeSubmit));

		static bool imGuiFontsBuilt = false;
		if (!imGuiFontsBuilt)
		{
			imGuiFontsBuilt = true;
			ImGui_ImplVulkan_CreateFontsTexture(cmd);
		}

		// Clear
		auto clearColor = vk::ClearColorValue(std::array<float,4>{ m_cameraPushC.ambiendColor.x(), m_cameraPushC.ambiendColor.y(), m_cameraPushC.ambiendColor.z(), 1.f });

		auto swapchainImage = renderContext().swapchainAquireNextImage(m_imageAvailableSemaphore, cmd);

		auto clearColorRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eColor, 0, 1, 0, 1);
		cmd.clearColorImage(
			swapchainImage,
			vk::ImageLayout::eGeneral,
			clearColor,
			clearColorRange);

		auto clearDepthRange = vk::ImageSubresourceRange(vk::ImageAspectFlagBits::eDepth, 0, 1, 0, 1);
		auto clearDepth = vk::ClearDepthStencilValue(0.f);
		cmd.clearDepthStencilImage(
			m_gBufferZ->image(),
			vk::ImageLayout::eGeneral,
			clearDepth,
			clearDepthRange);

		// Render the UI pass
		vk::RenderPassBeginInfo passInfo;
		passInfo.framebuffer = m_swapchainFrameBuffers[renderContext().currentFrameIndex()];
		passInfo.renderPass = m_uiPass;
		passInfo.renderArea.offset;
		passInfo.renderArea.extent.width = renderContext().windowSize().x();
		passInfo.renderArea.extent.height = renderContext().windowSize().y();
		passInfo.clearValueCount = 0;
		cmd.beginRenderPass(passInfo, vk::SubpassContents::eInline);

		// Render a triangle if the scene is loaded
		if (renderContext().allocator().isTransferFinished(m_sceneLoadStreamToken))
		{
			m_gBufferPipeline->bind(cmd);
			vk::Viewport viewport{ {0,0} };
			viewport.maxDepth = 1.0f;
			viewport.minDepth = 0.0f;
			viewport.width = (float)passInfo.renderArea.extent.width;
			viewport.height = (float)passInfo.renderArea.extent.height;
			cmd.setViewport(0, 1, &viewport);
			cmd.setScissor(0, passInfo.renderArea);

			// Update per instance model matrices
			auto mtxDst = renderContext().allocator().mapBuffer<math::Mat44f>(*m_mtxBuffers[m_doubleBufferNdx]);
			m_sceneInstances.updatePoses(mtxDst);
			renderContext().allocator().unmapBuffer(mtxDst);

			// Update descriptor set with this frame's matrices
			cmd.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_gbufferPipelineLayout, 0, m_frameDescs[m_doubleBufferNdx], {});

			// Update view and projection matrices push constants
			float aspect = viewport.width / viewport.height;
			m_cameraPushC.proj = mFlybyCam->projection(aspect);// .transpose();
			m_cameraPushC.view = mFlybyCam->view();// .transpose();

			cmd.pushConstants<CameraPushConstants>(
				m_gbufferPipelineLayout,
				vk::ShaderStageFlagBits::eVertex | vk::ShaderStageFlagBits::eFragment,
				0,
				m_cameraPushC);
			
			// Draw all instances in a single batch
			m_rasterData.bindBuffers(cmd);
			for (size_t i = 0; i < m_sceneInstances.numInstances(); ++i)
			{
				assert(i < std::numeric_limits<uint32_t>::max());

				auto meshNdx = m_sceneInstances.m_instanceMeshes[i];
				auto& mesh = m_sceneInstances.m_meshes[meshNdx];
				for (size_t primitiveId = mesh.first; primitiveId != mesh.second; ++primitiveId)
				{
					auto& primitive = m_rasterData.getPrimitiveById(primitiveId);
					cmd.drawIndexed(primitive.numIndices, 1, primitive.indexOffset, primitive.vtxOffset, (uint32_t)i);
				}
			}

			m_doubleBufferNdx ^= 1;
			
		}

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		auto& io = ImGui::GetIO();
		io.DisplaySize = { (float)windowSize().x(), (float)windowSize().y() };
		auto mousePos = input::PointingInput::get()->touchPosition();
		io.MousePos = { (float)mousePos.x(), (float)mousePos.y() };
		updateUI(dt.count());
		ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), cmd);

		cmd.endRenderPass();

		cmd.end();

		vk::PipelineStageFlags waitFlags = vk::PipelineStageFlagBits::eColorAttachmentOutput;
		vk::SubmitInfo submitInfo(
			1, &m_imageAvailableSemaphore, &waitFlags, // wait
			1, &cmd, // commands
			1, &renderContext().readyToPresentSemaphore()); // signal
		renderContext().graphicsQueue().submit(submitInfo);
		renderContext().swapchainPresent();

		// TODO:
		// Build render graph
			// Obtain swapchain image
			// Grab screen capture / record video frame

		/* Render pseudo code
		graph.reset();

		// Option A: Empty graph
		graph.addClearPass<format>(swapchain.next, Vec4f{0, 0, 0, 0}); // Syntax sugar for simple clears

		// Option B: Basic RT pass
		graphResId gbuffer;
		graphResId dirLight;
		graphResId toneMapped;
		graph.addRTPass(pipeline,
			[&](builder){
				gbuffer = builder.addColorOut(set, binding, format);
				dirLight = builder.addColorOut(set, binding, format);
				},
			[&](cmd){
				cmd.bind(uniforms);
				cmd.constants(pushConst);
				cmd.raytrace(size);
			});
		graph.addComputePass(pipeline,
			[&](builder){
				builder.addInput(set, binding, gbuffer);
				builder.addInput(set, binding, dirLight);
				builder.addOutput(set, binding, toneMapped);
				},
			[&](cmd){
				cmd.bind(uniforms);
				cmd.constants();
				cmd.dispatch(size);
			});

		// Common part. Everything up to here can be done only once per shader reload
		grpah.build(); // This will be invoke the build callbacks, and so potentially alter the graph
		graph.record(); // This will invoke the record callbacks, evaluate culling, Acceleration structs, ect...

		// Unsure about how this part should go.
		auto endSignal = graph.submit();
		queue.present();
		coawait fence(); // Not really needed here. Can actually do this wherever the resources are actually needed
		*/


		/*updateUI(dt.count());
		// Render scene
		mDeferred.render(mGraphicsScene, *mFlybyCam);
		// Render gui
		ImGui::Render();

		// Present to screen
		gfxDevice().renderQueue().present();*/
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::updateUI(float dt)
	{
		ImGui::NewFrame();

		if (ImGui::Begin("debug window"))
		{
			ImGui::Text("%s", "Sample text");
			ImGui::SliderFloat("Background", &m_bgColor, 0.f, 1.f);
			ImGui::ColorPicker3("Ambient Color", reinterpret_cast<float*>(&m_cameraPushC.ambiendColor));
			ImGui::ColorPicker3("Light Color", reinterpret_cast<float*>(&m_cameraPushC.lightColor));
		}
		ImGui::End();

		ImGui::Render();

		/*gui::startFrame(windowSize());

		if(ImGui::Begin("Player options"))
		{
			ImGui::InputFloat("Camera speed", &m_flyby->speed());
			ImGui::Checkbox("Floor", &m_floorGeom->visible);
		}
		ImGui::End();

		if(ImGui::Begin("Render options"))
		{
			ImGui::Checkbox("IBL Shadows", &m_bgOptions.shadows);
			if(m_bgOptions.shadows)
			{
				gui::slider("Shadow elevation", m_bgOptions.elevation, 0.f, math::Constants<float>::halfPi);
				gui::slider("Shadow rotation", m_bgOptions.rotation, 0.f, math::Constants<float>::twoPi);
			}
			m_envLight->castShadows = m_bgOptions.shadows;
			ImGui::SliderFloat("Exposure steps:", &mFlybyCam->exposure(), -5.f, 5.f);
		}
		ImGui::End();
		
		auto elevation = Quatf(normalize(Vec3f(1.f, 0.f, 0.f)), -m_bgOptions.elevation);
		auto rotation = Quatf(normalize(Vec3f(0.f, 1.f, 0.f)), m_bgOptions.rotation);

		AffineTransform lightXform = AffineTransform::identity();
		lightXform.setRotation(rotation * elevation);
		m_envLight->worldMatrix = lightXform;

		gui::finishFrame(dt);*/
	}


	//------------------------------------------------------------------------------------------------
	void Player::initImGui()
	{
		ImGui::CreateContext();

		auto& rc = renderContext();
		auto nativeWindow = rc.nativeWindow();
		ImGui_ImplWin32_Init(&nativeWindow);

		vk::DescriptorPoolSize pool_sizes[] =
		{
			{ vk::DescriptorType::eSampler, 1000 },
			{ vk::DescriptorType::eCombinedImageSampler, 1000 },
			{ vk::DescriptorType::eSampledImage, 1000 },
			{ vk::DescriptorType::eStorageImage, 1000 },
			{ vk::DescriptorType::eUniformTexelBuffer, 1000 },
			{ vk::DescriptorType::eStorageTexelBuffer, 1000 },
			{ vk::DescriptorType::eUniformBuffer, 1000 },
			{ vk::DescriptorType::eStorageBuffer, 1000 },
			{ vk::DescriptorType::eUniformBufferDynamic, 1000 },
			{ vk::DescriptorType::eStorageBufferDynamic, 1000 },
			{ vk::DescriptorType::eInputAttachment, 1000 }
		};

		vk::DescriptorPoolCreateInfo pool_info = {};
		pool_info.flags = vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet;
		pool_info.maxSets = 1000 * IM_ARRAYSIZE(pool_sizes);
		pool_info.poolSizeCount = (uint32_t)IM_ARRAYSIZE(pool_sizes);
		pool_info.pPoolSizes = pool_sizes;

		ImGui_ImplVulkan_InitInfo initInfo{};
		initInfo.Instance = rc.instance();
		initInfo.Device = rc.device();
		initInfo.PhysicalDevice = rc.physicalDevice();
		initInfo.Queue = rc.graphicsQueue();
		initInfo.QueueFamily = rc.graphicsQueueFamily();
		initInfo.Subpass = 0;
		initInfo.ImageCount = 2;
		initInfo.MinImageCount = 2;
		initInfo.MSAASamples = VK_SAMPLE_COUNT_1_BIT;
		initInfo.DescriptorPool = rc.device().createDescriptorPool(pool_info);

		ImGui_ImplVulkan_Init(&initInfo, m_uiPass);

		auto& os = *core::OSHandler::get();
		os += [=](MSG msg) {
			// ImGui handler
			ImGui_ImplWin32_WndProcHandler(nativeWindow, msg.message, msg.wParam, msg.lParam);
			return false;
		};
	}

}	// namespace rev