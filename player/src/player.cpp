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
#include <game/scene/gltf/gltfLoad.h>
#include <game/scene/transform/flyby.h>
#include <game/scene/transform/orbit.h>
#include <game/scene/transform/transform.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/renderer/material/material.h>
#include <graphics/renderer/material/Effect.h>
#include <graphics/scene/renderGeom.h>

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
			loadGLTFScene(*gltfRoot, scene, mGraphicsScene, *mGeometryPool);

			// Load sky
			if(!bg.empty())
			{
				mGraphicsScene.sky = Texture::load(bg+".hdr", false);
				mGraphicsScene.irradiance = Texture::load(bg+"_irradiance.hdr", false);
			}

			// Create camera
			createCamera();
			mGameScene.root()->init();

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
		// Orbit
		/*
		auto orbitNode = mGameScene.root()->createChild("camOrbit");
#ifdef ANDROID
		orbitNode->addComponent<Orbit>(Vec2f{-1e-3f,-1e-3f}, 1);
#else
		orbitNode->addComponent<Orbit>(Vec2f{-1e-2f,-1e-2f}, 1);
#endif
		orbitNode->addComponent<Transform>();

		// Cam node
		auto cameraNode = orbitNode->createChild("Camera");
		*/
		auto cameraNode = mGameScene.root()->createChild("Camera");
		cameraNode->addComponent<FlyBy>(10.f, 1.f);
		cameraNode->addComponent<Transform>()->xForm.position() = math::Vec3f { 0.f, -4.f, 0.f };
		mCamera = &cameraNode->addComponent<game::Camera>()->cam();
	}

	struct MeshHeader
	{
		uint32_t nVertices;
		uint32_t nIndices;
	};

	struct RenderObjData {
		int meshIdx = -1;
		Mat44f transform;
	};

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

		mRenderer.render(*mCamera, mGraphicsScene);

		gui::finishFrame(dt);
		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev