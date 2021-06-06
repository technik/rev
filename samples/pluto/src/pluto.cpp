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
#include <cassert>
#include <thread>

#define STB_IMAGE_IMPLEMENTATION
#include "pluto.h"
#include <math/algebra/vector.h>
#include <core/platform/fileSystem/file.h>
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/fileSystem/FolderWatcher.h>
#include <core/platform/cmdLineParser.h>
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

#include "ProceduralTerrain.h"

using namespace rev::math;
using namespace rev::gfx;
using namespace rev::game;

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	void Pluto::CommandLineOptions::registerOptions(core::CmdLineParser& args)
	{
		args.addOption("env", &environment);
		args.addOption("fov", &fov);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Pluto::getCommandLineOptions(core::CmdLineParser& args)
	{
		m_options.registerOptions(args);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Pluto::init()
	{
		core::FileSystem::get()->registerPath("./shaders");
		core::FileSystem::get()->registerPath("./data");

		// Create scene geometry
		m_sceneRoot = std::make_shared<SceneNode>("scene root");
		m_opaqueGeometry = std::make_shared<gfx::RasterScene>();
		float cellSide = 128.f;
		float cellHeight = 64.f;
		float meanHeight = 40.f;
		math::AABB cellBounds(
			Vec3f{ -cellSide * 0.5f, -meanHeight, -cellSide * 0.5f },
			Vec3f{ cellSide * 0.5f, cellHeight - meanHeight, cellSide * 0.5f });

		uint32_t gridSide = 128;
		uint32_t gridHeight = 32;
		ProceduralTerrain::generateMarchingCubes(cellBounds, 0.f, gridSide, gridHeight, *m_opaqueGeometry);
		m_geometryStreamToken = m_opaqueGeometry->m_geometry.closeAndSubmit(RenderContext(), RenderContext().allocator());
		m_sceneGraphics.m_opaqueGeometry.push_back(m_opaqueGeometry);

		// Create camera
		createCamera();
		m_sceneRoot->init();

		// Lighting state
		m_sceneGraphics.ambientColor = Vec3f(0.5f/255, 16/255.f, 20.f/255);
		m_sceneGraphics.lightColor = Vec3f(1.f, 1.f, 1.f);
		m_sceneGraphics.lightDir = normalize(Vec3f(1.f, 1.f, 1.f));

		// Init renderer
		gfx::DeferredRenderer::Budget rendererLimits;
		rendererLimits.maxTexturesPerBatch = (uint32_t)16;
		m_renderer.init(
			renderContext(),
			renderContext().windowSize(),
			rendererLimits,
			"./shaders"
		);

		m_opaqueGeometry->updateDescriptorSet(m_renderer.batchDescriptorLayout());

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Pluto::end()
	{
		m_renderer.end();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Pluto::onResize()
	{
		m_renderer.onResize(windowSize());
	}

		//------------------------------------------------------------------------------------------------------------------
	void Pluto::createCamera()
	{	
		// Create flyby camera
		auto cameraNode = m_sceneRoot->createChild("Flyby cam");
		m_flyby = cameraNode->addComponent<FlyBy>(2.f, 1.f);
		cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { 0.0f, 0.f, 9.f };
		auto camComponent = cameraNode->addComponent<game::Camera>(math::Pi/5, 0.01f, 5000.f);
		mFlybyCam = &*camComponent->cam();
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Pluto::updateLogic(TimeDelta dt)
	{
		// TODO: Play animations
		m_sceneRoot->update(dt.count());
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Pluto::render(TimeDelta dt)
	{
		Vec2f windowSize = { (float)renderContext().windowSize().x(), (float)renderContext().windowSize().y() };
		float aspect = windowSize.x() / windowSize.y();

		m_sceneGraphics.proj = mFlybyCam->projection(aspect);
		m_sceneGraphics.view = mFlybyCam->view();

		ImGui_ImplWin32_NewFrame();
		ImGui_ImplVulkan_NewFrame();
		auto& io = ImGui::GetIO();
		io.DisplaySize = { windowSize.x(), windowSize.y() };
		auto mousePos = input::PointingInput::get()->touchPosition();
		io.MousePos = { (float)mousePos.x(), (float)mousePos.y() };
		updateUI(dt.count());

		if (m_sceneGraphics.m_opaqueGeometry.empty())
		{
			if (RenderContext().allocator().isTransferFinished(m_geometryStreamToken))
			{
				m_sceneGraphics.m_opaqueGeometry.push_back(m_opaqueGeometry);
			}
		}

		m_renderer.render(m_sceneGraphics);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Pluto::updateUI(float dt)
	{
		ImGui::NewFrame();

		if (ImGui::Begin("debug window"))
		{
			if (ImGui::CollapsingHeader("Lighting"))
			{
				ImGui::ColorPicker3("Ambient Color", reinterpret_cast<float*>(&m_sceneGraphics.ambientColor));
				ImGui::ColorPicker3("Light Color", reinterpret_cast<float*>(&m_sceneGraphics.lightColor));
				ImGui::SliderFloat3("Light dir", reinterpret_cast<float*>(&m_sceneGraphics.lightDir), -3.f, 3.f);
			}
			m_renderer.updateUI();
		}
		ImGui::End();

		ImGui::Render();
	}

}	// namespace rev