//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-31
//----------------------------------------------------------------------------------------------------------------------
// Sample to demonstrate deferred rendering.

#include <engine.h>
#include <game/scene/camera/flyByCamera.h>
#include <game/geometry/procedural/basic.h>
#include <input/keyboard/keyboardInput.h>
#include <math/algebra/vector.h>
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
#include <vector>
#include <iostream>

using namespace rev::core;
using namespace rev::game;
using namespace rev::math;
using namespace rev::video;

class SceneDemo : public rev::App3d {
public:
	SceneDemo(int _argc, const char** _argv)
		: App3d(_argc, _argv)
	{
		processArgs(_argc, _argv);

		// Construct global objects
		mRenderer = new ForwardRenderer();
		mRenderer->init<NewAllocator>(&driver3d(), mAlloc);
		mCam = new FlyByCamera(1.57f, 4.f/3.f, 0.001f, 10000.f);
		mCam->setPos({0.f, -10.f, 1.72f});
		mWorld = new PhysicsWorld();

		// Load scene
		mScene = Scene::import("sample.DAE");
		mScene->mRenderContext->setCamera(mCam);
		RenderObj* groundRo;
		RenderObj* ballRo;
		SceneNode* ground = mScene->mSceneGraph["Floor"];
		SceneNode* ball = mScene->mSceneGraph["Ball"];
		for (auto ro : *mScene->mRenderContext) {
			if(ro->node() == ground)
				groundRo = ro;
			if(ro->node() == ball)
				ballRo = ro;
		}

		// Configure ground
		RigidBody* groundBd = RigidBody::box(0.f, groundRo->mBBox.max - groundRo->mBBox.min);
		groundBd->setPosition(ground->position());
		ground->attachTo(groundBd);
		mWorld->addRigidBody(groundBd);

		// Configure ball
		mBallBd = RigidBody::sphere(1.f, ballRo->mBBox.max.x);
		mBallBd->setPosition(ball->position());
		ball->attachTo(mBallBd);
		mWorld->addRigidBody(mBallBd);
	}

	~SceneDemo() {
		mRenderer->end<NewAllocator>(mAlloc);
	}

	ForwardRenderer* mRenderer;
	Scene* mScene;
	NewAllocator mAlloc;
	FlyByCamera* mCam;
	PhysicsWorld* mWorld;
	RigidBody* mBallBd;
	float t = 0;

private:
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		_argc;
		_argv;
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {
		t += _dt;
		if(t < 2.f)
			std::cout << mBallBd->position().z << "\n";
		// Update objects in the middle
		mCam->update(_dt);

		if(keyboard().pressed(rev::input::KeyboardInput::eSpace))
			mBallBd->setPosition({0.f,0.f,5.f});
		mWorld->simulate(_dt);

		mRenderer->renderContext(*mScene->mRenderContext);
		return true;
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	SceneDemo app(_argc,_argv);

	while(app.update());
	return 0;
}