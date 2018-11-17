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
	bool Player::init(const math::Vec2u& windowSize, const std::string& scene, const std::string& bg) {
		core::Time::init();

		// Create geometry pool
		mGeometryPool = std::make_unique<GeometryPool>();
		// Load scene
		auto gltfRoot = std::make_shared<SceneNode>("gltf scene parent");
		//gltfRoot->addComponent<Orbit>(Vec2f{0.1f, 0.1f});
		mGameScene.root()->addChild(gltfRoot);
		auto rotation = math::Mat33f({
			-1.f, 0.f, 0.f,
			0.f, 0.f, 1.f,
			0.f, 1.f, 0.f
			});
		auto xForm = gltfRoot->addComponent<Transform>();
		//xForm->xForm.rotate(rotation);

		std::vector<std::shared_ptr<Animation>> animations;
		std::vector<std::shared_ptr<SceneNode>> animNodes;
		loadGLTFScene(m_gfx, *gltfRoot, scene, mGraphicsScene, *mGeometryPool, animNodes, animations);

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

		// Create animation component
		game::Animator* animator = nullptr;
		if(animations.size() > 0)
		{
			//animator = animNodes[0]->addComponent<Animator>();
		}

		// Create camera
		createCamera();
		createFloor();
		mGameScene.root()->init();
		if(animator)
			animator->playAnimation(animations[0], true);

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
	void Player::createCamera() {
		if(mGraphicsScene.cameras().empty())
		{
			// Create default camera
			auto cameraNode = mGameScene.root()->createChild("Camera");
			cameraNode->addComponent<FlyBy>(2.f, 1.f);
			cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { -2.5f, 1.f, 3.f };
			cameraNode->component<Transform>()->xForm.rotate(Quatf({0.f,1.f,0.f}, -0.5f*Constants<float>::halfPi));
			auto camComponent = cameraNode->addComponent<game::Camera>();
			mCamera = &*camComponent->cam();
		}
		else
		{
			mCamera = &*mGraphicsScene.cameras()[0].lock();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createFloor() {

		auto floorNode = mGameScene.root()->createChild("floor");
		floorNode->addComponent<Transform>()->xForm.rotate(Quatf({1.f,0.f,0.f}, -math::Constants<float>::halfPi));

		auto floorMesh = std::make_shared<gfx::RenderGeom>(RenderGeom::quad({10.f, 10.f}));

		// Create default material
		auto pbrEffect = std::make_shared<Effect>("../data/shaders/metal-rough.fx");
		auto defaultMaterial = std::make_shared<Material>(pbrEffect);

		gfx::TextureSampler::Descriptor samplerDesc;
		samplerDesc.wrapS = gfx::TextureSampler::Descriptor::Wrap::Clamp;
		samplerDesc.wrapT = gfx::TextureSampler::Descriptor::Wrap::Clamp;
		auto sampler = m_gfx.createTextureSampler(samplerDesc);
		auto envBRDF = load2dTextureFromFile(m_gfx, sampler, "../data/shaders/ibl_brdf.hdr", false, 1);
		defaultMaterial->addTexture("uEnvBRDF", envBRDF);

		// Create mesh renderer component
		auto renderable = std::make_shared<gfx::RenderMesh>();
		renderable->mPrimitives.push_back({floorMesh, defaultMaterial});
		auto renderObj = std::make_shared<gfx::RenderObj>(renderable);
		mGraphicsScene.renderables().push_back(renderObj);

		floorNode->addComponent<game::MeshRenderer>(renderObj);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update()
	{
		core::Time::get()->update();
		auto dt = core::Time::get()->frameTime();

		updateUI(dt);

		mGameScene.root()->update(dt);

		// Render
		mRenderer.render(mGraphicsScene, *mCamera);
		ImGui::Render();
		m_gfx.renderQueue().present();

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::updateUI(float dt)
	{
		gui::startFrame(m_windowSize);

		if(ImGui::Begin("Environment"))
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