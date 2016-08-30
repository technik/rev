//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Sample racing game

#include <engine.h>
#include <game/geometry/procedural/basic.h>
#include <input/keyboard/keyboardInput.h>
#include <math/algebra/vector.h>
#include <math/geometry/types.h>
#include <util/app/app3d.h>
#include <video/basicTypes/color.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderer/renderContext.h>
#include <video/graphics/renderer/material.h>
#include <video/graphics/renderer/renderMesh.h>
#include <video/graphics/renderer/renderObj.h>
#include <core/memory/newAllocator.h>
#include <core/components/sceneNode.h>
#include <game/physics/rigidBody.h>
#include <game/physics/physicsWorld.h>
#include <game/scene/scene.h>
#include <game/scene/camera/flyByCamera.h>
#include <input/keyboard/keyboardInput.h>
#include <video/basicTypes/texture.h>
#include <video/graphics/renderer/material.h>
#include <vector>
#include <iostream>

using namespace rev::core;
using namespace rev::game;
using namespace rev::input;
using namespace rev::math;
using namespace rev::video;

class RacingDemo : public rev::App3d {
public:
	RacingDemo(int _argc, const char** _argv)
		: App3d(_argc, _argv)
	{
		processArgs(_argc, _argv);

		// Construct global objects
		mKey = KeyboardInput::get();
		mRenderer = new ForwardRenderer();
		mRenderer->init<NewAllocator>(&driver3d(), mAlloc);
		mRenderer->setWindowSize(window().size());
		mWorld = new PhysicsWorld();
		mScene = Scene::import("scene.DAE");

		// Load skybox
		mSkyMaterial.mDiffMap = Texture::load("skybox.jpg", &driver3d());
		mScene->mRenderContext->skyBox = new RenderObj(Procedural::box(Vec3f(1.f)));
		mScene->mRenderContext->skyBox->mMaterial = &mSkyMaterial;
		mScene->mRenderContext->skyBox->attachTo(new SceneNode);
		mScene->mRenderContext->skyBox->mesh()->uvs[0] = Vec2f(0.75f, 1.f/3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[1] = Vec2f(0.75f, 2.f/3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[2] = Vec2f(0.5f, 2.f/3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[3] = Vec2f(0.5f, 1.f/3.f);

		mScene->mRenderContext->skyBox->mesh()->uvs[4] = Vec2f(0.25f, 1.f/3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[5] = Vec2f(0.5f, 1.f / 3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[6] = Vec2f(0.5f, 2.f / 3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[7] = Vec2f(0.25f, 2.f / 3.f);

		//mScene->mRenderContext->skyBox->mesh()->uvs[ 8] = Vec2f(1.f, 0.f);
		//mScene->mRenderContext->skyBox->mesh()->uvs[ 9] = Vec2f(1.f, 0.f);
		//mScene->mRenderContext->skyBox->mesh()->uvs[10] = Vec2f(1.f, 0.f);
		//mScene->mRenderContext->skyBox->mesh()->uvs[11] = Vec2f(1.f, 0.f);
		
		mScene->mRenderContext->skyBox->mesh()->uvs[12] = Vec2f(0.f, 2.f / 3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[13] = Vec2f(0.f, 1.f / 3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[14] = Vec2f(0.25f, 1.f / 3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[15] = Vec2f(0.25f, 2.f / 3.f);

		mScene->mRenderContext->skyBox->mesh()->uvs[16] = Vec2f(0.75f, 1.f / 3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[17] = Vec2f(1.f, 1.f / 3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[18] = Vec2f(1.f, 2.f / 3.f);
		mScene->mRenderContext->skyBox->mesh()->uvs[19] = Vec2f(0.75f, 2.f / 3.f);

		//mScene->mRenderContext->skyBox->mesh()->uvs[20] = Vec2f(0.f, 0.f);
		//mScene->mRenderContext->skyBox->mesh()->uvs[21] = Vec2f(0.f, 0.f);
		//mScene->mRenderContext->skyBox->mesh()->uvs[22] = Vec2f(0.f, 0.f);
		//mScene->mRenderContext->skyBox->mesh()->uvs[23] = Vec2f(0.f, 0.f);
		
		mCam = new FlyByCamera(1.5f, 1.333f, 0.1f, 100.f);
		mDebugCam = new FlyByCamera(1.5f, 1.333f, 0.1f, 500.f);
		mCam->setPos({0.f,0.f,1.f});
		mDebugCam->setPos({ 0.f,0.f,1.f });

		mScene->mRenderContext->setCamera(mCam);

		Vec3f groundSize = {1000.f, 1000.f, 2.f};
		RigidBody* groundBd = RigidBody::box(0.f, groundSize);
		RenderObj* groundRo = new RenderObj(Procedural::box(groundSize));
		groundRo->castShadows = false;
		SceneNode* ground = new SceneNode;
		ground->attachTo(groundBd);
		groundRo->attachTo(ground);
		groundBd->setPosition({0.f,0.f,-groundSize.z*0.5f});
		mWorld->addRigidBody(groundBd);
		mScene->mRenderContext->insert(groundRo);

		// Obstacles
		for(auto i = 0; i < 10; ++i)
			for (auto j = 0; j < 8; ++j)
				createCube({-22.f+4.f*i, 20.f-2*j, 0.5f+0.1f*i}, mWorld, mScene->mRenderContext);
	}

	~RacingDemo() {
		mRenderer->end<NewAllocator>(mAlloc);
	}

	KeyboardInput* mKey;
	FlyByCamera* mCam;
	FlyByCamera* mDebugCam;
	ForwardRenderer* mRenderer;
	Scene* mScene;
	NewAllocator mAlloc;
	PhysicsWorld* mWorld;

	Material mSkyMaterial;
	float t = 0;
	bool mDebug = false;
	bool mDebugMove = false;

private:
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		_argc;
		_argv;
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {
		t += _dt;

		if (mKey->pressed(KeyboardInput::eTab)) {
			if (!mDebug) {
				mDebug = true;
				mDebugMove = true;
			}
			else {
				mDebug = false;
				mDebugMove = false;
			}
		}

		if (mKey->pressed(KeyboardInput::eQ) && mDebug) {
			mDebugMove = !mDebugMove;
		}


		if(mDebugMove)
			mDebugCam->update(_dt);
		else
			mCam->update(_dt);

		if(mDebug) {
			mRenderer->setDebugCamera(mDebugCam);
		}
		else {
			mRenderer->setDebugCamera(nullptr);
		}
		mWorld->simulate(_dt);


		mRenderer->renderContext(*mScene->mRenderContext);
		mRenderer->finishFrame();
		return true;
	}

	//----------------------------------------------------------------
	void createCube(const Vec3f& _pos, PhysicsWorld* _w, RenderContext* _renderCtxt) {
		SceneNode* node = new SceneNode();
		Vec3f size = Vec3f(1.f);
		RigidBody* bd = RigidBody::box(0.f, size);
		_w->addRigidBody(bd);
		RenderObj* model = new RenderObj(Procedural::box(size));
		_renderCtxt->insert(model);
		model->attachTo(node);
		node->attachTo(bd);
		bd->setPosition(_pos);
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	RacingDemo app(_argc,_argv);

	while(app.update());
	return 0;
}