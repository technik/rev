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
#include <game/scene/gltf/gltfLoad.h>
#include <game/scene/transform/flyby.h>
#include <game/scene/transform/orbit.h>
#include <game/scene/transform/transform.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/renderer/material/material.h>
#include <graphics/renderer/material/Effect.h>
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
		auto sceneLight = std::make_shared<gfx::DirectionalLight>();

		// Default scene light
		{
			AffineTransform lightXform = AffineTransform::identity();
			lightXform.setRotation(Quatf(normalize(Vec3f(1.f, 0.f, 0.f)), Constants<float>::halfPi*0.4));
			sceneLight->worldMatrix = lightXform;
			sceneLight->color = 4*Vec3f::ones();
			sceneLight->castShadows = true;
			mGraphicsScene.addLight(sceneLight);
		}

		// Load sky
		if(!bg.empty())
		{
			auto probe = std::make_shared<EnvironmentProbe>(m_gfx, bg+".json");
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
		mGameScene.root()->init();
		if(animator)
			animator->playAnimation(animations[0], true);

		mRenderer.init(m_gfx, windowSize, m_gfx.defaultFrameBuffer());
		gui::init(windowSize);

		return true;
	}

#ifdef _WIN32
	//------------------------------------------------------------------------------------------------------------------
	void Player::onWindowResize(const math::Vec2u& _newSize)
	{
		mRenderer.onResizeTarget(_newSize);
	}
#endif // _WIN32

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera() {
		if(mGraphicsScene.cameras().empty())
		{
			// Create default camera
			auto cameraNode = mGameScene.root()->createChild("Camera");
			cameraNode->addComponent<FlyBy>(10.f, 1.f);
			cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { 0.f, 4.f, 19.f };
			auto camComponent = cameraNode->addComponent<game::Camera>();
			mCamera = &*camComponent->cam();
			//cameraNode->component<Transform>()->xForm.rotate(Quatf({1.f,0.f,0.f}, Constants<float>::halfPi));
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update()
	{
		core::Time::get()->update();
		//gui::startFrame(->frameBuffer()->size());

		auto dt = core::Time::get()->frameTime();

		mGameScene.root()->update(dt);

		//mRenderer.showDebugInfo(mGameEditor.mShowRenderOptions);
		mRenderer.render(mGraphicsScene, *mCamera);

		//gui::finishFrame(dt);
		m_gfx.renderQueue().present();
		//mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev