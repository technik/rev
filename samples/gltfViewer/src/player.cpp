//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#define STB_IMAGE_IMPLEMENTATION
#include "player.h"
#include <math/algebra/vector.h>
#include <core/platform/fileSystem/file.h>
#include <core/time/time.h>
#include <core/tools/log.h>
#include <game/scene/camera.h>
#include <game/animation/animator.h>
#include <game/resources/load.h>
#include <game/scene/gltf/gltfLoad.h>
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
	bool Player::init(const math::Vec2u& windowSize, const std::string& scene, const std::string& bg)
	{
		core::Time::init();
				
		loadScene(scene);

		// Default scene light
		{
			m_envLight = std::make_shared<gfx::DirectionalLight>();
			m_envLight->castShadows = true;
			AffineTransform lightXform = AffineTransform::identity();
			lightXform.setRotation(Quatf(normalize(Vec3f(1.f, 0.f, 0.f)), Constants<float>::halfPi*0.4));
			m_envLight->worldMatrix = lightXform;
			m_envLight->color = 4*Vec3f::ones();
			m_envLight->castShadows = true;
			mGraphicsScene.addLight(m_envLight);
		}

		// Load sky
		if(!bg.empty())
		{
			auto probe = std::make_shared<EnvironmentProbe>(m_gfx, bg);
			if(probe->numLevels())
				mGraphicsScene.setEnvironment(probe);
		}

		// Create camera
		createCamera();
		createFloor();
		mGameScene.root()->init();

		mRenderer.init(m_gfx, windowSize, m_gfx.defaultFrameBuffer());
		onWindowResize(windowSize); // Hack: This shouldn't be necessary, but aparently the renderer doesn't initialize properly.
		gui::init(windowSize);

		return true;
	}

#ifdef _WIN32
	//------------------------------------------------------------------------------------------------------------------
	void Player::onWindowResize(const math::Vec2u& _newSize)
	{
		m_windowSize = _newSize;
		mRenderer.onResizeTarget(_newSize);
	}
#endif // _WIN32

	//------------------------------------------------------------------------------------------------------------------
	void Player::loadScene(const std::string& scene)
	{
		m_gltfRoot = std::make_shared<SceneNode>("gltf scene parent");
		m_gltfRoot->addComponent<Transform>();
		mGameScene.root()->addChild(m_gltfRoot);

		std::vector<std::shared_ptr<Animation>> animations;
		std::vector<std::shared_ptr<SceneNode>> animNodes;
		loadGLTFScene(m_gfx, *m_gltfRoot, scene, mGraphicsScene, animNodes, animations);
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
				auto wsBbox = transform->absoluteXForm() * localbbox;
				m_globalBBox.add(wsBbox);
			}
		});

		// Re-center scene
		auto xForm = m_gltfRoot->component<Transform>();
		xForm->xForm.position() = -m_globalBBox.origin();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera() {
		
		// Create fliby camera
		auto cameraNode = mGameScene.root()->createChild("Flyby cam");
		m_flyby = cameraNode->addComponent<FlyBy>(2.f, 1.f);
		cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { -2.5f, 1.f, 3.f };
		cameraNode->component<Transform>()->xForm.rotate(Quatf({0.f,1.f,0.f}, -0.5f*Constants<float>::halfPi));
		auto camComponent = cameraNode->addComponent<game::Camera>(math::Pi/4, 0.01f, 1000.f);
		mFlybyCam = &*camComponent->cam();
		
		// Create orbit camera
		cameraNode = mGameScene.root()->createChild("Orbit cam");
		m_orbit = cameraNode->addComponent<Orbit>(Vec2f{2.f, 1.f});
		cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { -2.5f, 1.f, 3.f };
		cameraNode->component<Transform>()->xForm.rotate(Quatf({0.f,1.f,0.f}, -0.5f*Constants<float>::halfPi));
		camComponent = cameraNode->addComponent<game::Camera>(math::Pi/4, 0.01f, 1000.f);
		mOrbitCam = &*camComponent->cam();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createFloor() {

		auto floorNode = mGameScene.root()->createChild("floor");
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
		samplerDesc.wrapS = gfx::TextureSampler::Descriptor::Wrap::Clamp;
		samplerDesc.wrapT = gfx::TextureSampler::Descriptor::Wrap::Clamp;
		auto sampler = m_gfx.createTextureSampler(samplerDesc);
		auto envBRDF = load2dTextureFromFile(m_gfx, sampler, "shaders/ibl_brdf.hdr", false, 1);
		defaultMaterial->addTexture("uEnvBRDF", envBRDF);

		// Create mesh renderer component
		auto renderable = std::make_shared<gfx::RenderMesh>();
		renderable->mPrimitives.push_back({floorMesh, defaultMaterial});
		m_floorGeom = std::make_shared<gfx::RenderObj>(renderable);
		m_floorGeom->visible = false;
		mGraphicsScene.renderables().push_back(m_floorGeom);

		floorNode->addComponent<game::MeshRenderer>(m_floorGeom);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update()
	{
		core::Time::get()->update();
		auto dt = core::Time::get()->frameTime();

		updateUI(dt);

		mGameScene.root()->update(dt);

		// Render
		mRenderer.render(mGraphicsScene, *mFlybyCam);
		ImGui::Render();
		m_gfx.renderQueue().present();

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::updateUI(float dt)
	{
		gui::startFrame(m_windowSize);

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

				gui::slider("Shadow bias", mRenderer.shadowBias(), -0.1f, 0.1f);
			}
			m_envLight->castShadows = m_bgOptions.shadows;
		}
		ImGui::End();
		mRenderer.drawDebugUI();
		
		auto elevation = Quatf(normalize(Vec3f(1.f, 0.f, 0.f)), -m_bgOptions.elevation);
		auto rotation = Quatf(normalize(Vec3f(0.f, 1.f, 0.f)), m_bgOptions.rotation);

		AffineTransform lightXform = AffineTransform::identity();
		lightXform.setRotation(rotation * elevation);
		m_envLight->worldMatrix = lightXform;

		gui::finishFrame(dt);
	}

}	// namespace rev