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
		m_sceneRoot = std::make_shared<SceneNode>("scene root");

		// Load scene geometry
		loadScene(m_options.scene);

		// Create camera
		createCamera();
		m_sceneRoot->init();

		// Lighting state
		m_sceneGraphics.ambientColor = Vec3f(0.2f, 0.2f, 0.4f);
		m_sceneGraphics.lightColor = Vec3f(1.f, 1.f, 1.f);
		m_sceneGraphics.lightDir = normalize(Vec3f(1.f, 1.f, 1.f));

		// Init renderer
		auto sceneStream = m_renderer.init(
			renderContext(),
			renderContext().windowSize(),
			m_sceneGraphics
		);

		if (sceneStream)
			m_sceneLoadStreamToken = sceneStream;

		return true;
	}
	//------------------------------------------------------------------------------------------------------------------
	void Player::end()
	{
		m_renderer.end();
	}

#ifdef _WIN32
	//------------------------------------------------------------------------------------------------------------------
	void Player::onResize()
	{
		m_renderer.onResize(windowSize());
	}
#endif // _WIN32

	//------------------------------------------------------------------------------------------------------------------
	void Player::loadScene(const std::string& scene)
	{
		if (scene.empty()) // Early out
			return;

		GltfLoader gltfLoader(renderContext());
		auto loadRes = gltfLoader.load(scene, m_sceneGraphics.m_rasterData);
		m_sceneGraphics.m_sceneInstances = std::move(loadRes.meshInstances);
		m_sceneGraphics.m_materials = std::move(loadRes.materials);

		m_sceneRoot->addChild(loadRes.rootNode);
		m_sceneLoadStreamToken = m_sceneGraphics.m_rasterData.closeAndSubmit(renderContext(), renderContext().allocator());
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
	bool Player::updateLogic(TimeDelta dt)
	{
		// TODO: Play animations
		m_sceneRoot->update(dt.count());
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::render(TimeDelta dt)
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

		m_renderer.render(m_sceneGraphics, renderContext().allocator().isTransferFinished(m_sceneLoadStreamToken));
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::updateUI(float dt)
	{
		ImGui::NewFrame();

		if (ImGui::Begin("debug window"))
		{
			if (ImGui::CollapsingHeader("Lighting"))
			{
				ImGui::ColorPicker3("Ambient Color", reinterpret_cast<float*>(&m_sceneGraphics.ambientColor));
				ImGui::ColorPicker3("Light Color", reinterpret_cast<float*>(&m_sceneGraphics.lightColor));
				ImGui::DragFloat3("Light dir", reinterpret_cast<float*>(&m_sceneGraphics.lightDir), 1.f, -3.f, 3.f);
			}
			m_renderer.updateUI();
		}
		ImGui::End();

		ImGui::Render();
	}

}	// namespace rev