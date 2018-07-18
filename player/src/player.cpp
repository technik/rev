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
using namespace rev::graphics;
using namespace rev::game;

namespace rev {

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window, const std::string& scene, const std::string& bg) {
		core::Time::init();

		assert(!mGfxDriver);
		mGfxDriver = GraphicsDriverGL::createDriver(_window);
		if(mGfxDriver) {
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
			xForm->xForm.rotate(rotation);

			std::vector<std::shared_ptr<Animation>> animations;
			std::vector<std::shared_ptr<SceneNode>> animNodes;
			loadGLTFScene(*gltfRoot, scene, mGraphicsScene, *mGeometryPool, animNodes, animations);
			auto sceneLight = std::make_shared<graphics::DirectionalLight>();

			// Default scene light
			{
				AffineTransform lightXform = AffineTransform::identity();
				lightXform.setRotation(Quatf(Vec3f(1.f, 0.3f, 0.f).normalized(), -Constants<float>::halfPi));
				sceneLight->worldMatrix = lightXform;
				sceneLight->color = Vec3f::ones();
				sceneLight->castShadows = true;
				mGraphicsScene.addLight(sceneLight);
			}

			// Load sky
			if(!bg.empty())
			{
				mGraphicsScene.sky = Texture::load(bg+".hdr", false);
				mGraphicsScene.irradiance = Texture::load(bg+"_irradiance.hdr", false);
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

			mGameEditor.init(mGraphicsScene);
			mRenderer.init(*mGfxDriver, *mGfxDriver->frameBuffer());
			gui::init(_window->size);
		}
		return mGfxDriver != nullptr;
	}

#ifdef _WIN32
	//------------------------------------------------------------------------------------------------------------------
	void Player::onWindowResize(const math::Vec2u& _newSize)
	{
		if(mGfxDriver)
			mGfxDriver->onWindowResize(_newSize);
	}
#endif // _WIN32

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera() {
		if(mGraphicsScene.cameras().empty())
		{
			// Create default camera
			auto cameraNode = mGameScene.root()->createChild("Camera");
			cameraNode->addComponent<FlyBy>(10.f, 1.f);
			cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { 0.f, -4.f, 0.f };
			cameraNode->addComponent<game::Camera>(&mGraphicsScene);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update()
	{
		if(!mGfxDriver)
			return true;
		core::Time::get()->update();
		gui::startFrame(mGfxDriver->frameBuffer()->size());

		mGameEditor.update(mGameScene);

		auto dt = core::Time::get()->frameTime();

		mGameScene.root()->update(dt);

		mRenderer.render(mGraphicsScene);

		gui::finishFrame(dt);
		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev