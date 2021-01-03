//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "player.h"
#include <math/algebra/vector.h>
#include <core/platform/fileSystem/file.h>
#include <core/platform/cmdLineParser.h>
#include <core/tools/log.h>
#include <game/scene/camera.h>
#include <game/animation/animator.h>
#include <game/resources/load.h>
#include <game/scene/gltf/gltfLoader.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/transform/flyby.h>
#include <game/scene/transform/orbit.h>
#include <game/scene/transform/transform.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/renderer/material/material.h>
#include <graphics/renderer/material/Effect.h>
#include <graphics/scene/renderMesh.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/scene/animation/animation.h>

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
		// Create render context
		// 
		/*
		mDeferred.init(gfxDevice(), windowSize(), backBuffer());
		loadScene(m_options.scene);

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

		// Create camera
		createCamera();
		createFloor();
		mGameScene.root()->init();

		gui::init(windowSize());
		*/
		return true;
	}

#ifdef _WIN32
	//------------------------------------------------------------------------------------------------------------------
	void Player::onResize()
	{
		//mDeferred.onResizeTarget(windowSize());
	}
#endif // _WIN32

	//------------------------------------------------------------------------------------------------------------------
	void Player::loadScene(const std::string& scene)
	{
		// TODO:
		// Preload metadata and scene definition
		// Allocate memory in the renderer for meshes (and maybe textures)
		// Load scene (meshes, mats, etc)
		// Update scene representation in GUI
		// Optimize/Convert buffers to runtime formats
		// Instantiate nodes with render components
		// Load animations, if any
		// Load lights
		// Load cameras

		m_gltfRoot = std::make_shared<SceneNode>("gltf scene parent");
		m_gltfRoot->addComponent<Transform>();
		mGameScene.root()->addChild(m_gltfRoot);

		std::vector<std::shared_ptr<Animation>> animations;
		std::vector<std::shared_ptr<SceneNode>> animNodes;
		GltfLoader gltfLoader(gfxDevice());
		gltfLoader.load(*m_gltfRoot, scene, mGraphicsScene, animNodes, animations);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::updateSceneBBox()
	{
		// Compute scene bbox
		m_globalBBox.clear();
		m_gltfRoot->traverseSubtree([&](SceneNode& node){
			if(auto renderer = node.component<game::MeshRenderer>())
			{
				auto localbbox = renderer->renderObj().mesh->m_bbox;
				auto transform = node.component<Transform>();
				auto wsBbox = transform->absoluteXForm().matrix() * localbbox;
				m_globalBBox.add(wsBbox);
			}
		});

		// Re-center scene
		auto xForm = m_gltfRoot->component<Transform>();
		xForm->xForm.position() = -m_globalBBox.center();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera()
	{	
		// Create flyby camera
		auto cameraNode = mGameScene.root()->createChild("Flyby cam");
		m_flyby = cameraNode->addComponent<FlyBy>(2.f, 1.f);
		cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { 0.0f, 0.f, 9.f };
		auto camComponent = cameraNode->addComponent<game::Camera>(math::Pi/5, 0.01f, 5000.f);
		mFlybyCam = &*camComponent->cam();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createFloor()
	{
		auto floorNode = mGameScene.root()->createChild("floor");
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

		floorNode->addComponent<game::MeshRenderer>(m_floorGeom);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::updateLogic(TimeDelta dt)
	{
		// TODO: Play animations
		mGameScene.root()->update(dt.count());
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::render(TimeDelta dt)
	{
		// TODO:
		// Obtain swapchain image
		// Build render graph
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
		gui::startFrame(windowSize());

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

		gui::finishFrame(dt);
	}

}	// namespace rev