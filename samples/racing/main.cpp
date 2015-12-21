//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Sample racing game

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

class RacingDemo : public rev::App3d {
public:
	RacingDemo(int _argc, const char** _argv)
		: App3d(_argc, _argv)
	{
		processArgs(_argc, _argv);

		// Construct global objects
		mRenderer = new ForwardRenderer();
		mRenderer->init<NewAllocator>(&driver3d(), mAlloc);
		mRenderer->setWindowSize(window().size());
		mWorld = new PhysicsWorld();
		mScene = new Scene();
	}

	~RacingDemo() {
		mRenderer->end<NewAllocator>(mAlloc);
	}

	ForwardRenderer* mRenderer;
	Scene* mScene;
	NewAllocator mAlloc;
	PhysicsWorld* mWorld;
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
		mWorld->simulate(_dt);

		mRenderer->renderContext(*mScene->mRenderContext);
		return true;
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	RacingDemo app(_argc,_argv);

	while(app.update());
	return 0;
}