//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Sample racing game

#include <util/app/app3d.h>
#include <iostream>
#include <game/sceneLoader.h>
#include <video/basicTypes/camera.h>
#include <video/graphics/material.h>
#include <video/graphics/renderObj.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/staticRenderMesh.h>
#include <video/graphics/renderer/renderLayer.h>
#include <cjson/json.h>
#include <core/world/world.h>

using namespace rev::core;
using namespace rev::game;
using namespace rev::input;
using namespace rev::math;
using namespace rev::video;

using namespace std;

class VRSample : public rev::App3d {
public:
	VRSample(int _argc, const char** _argv)
		: App3d(_argc, _argv)
	{
		processArgs(_argc, _argv);
		mRenderer.init(&driver3d());
		SceneLoader mLoader;

		mRenderScene = new RenderLayer(mRenderer);
		mGameWorld.addLayer(mRenderScene);
		mLoader.registerFactory("RenderObj", [=](const cjson::Json& _j) { return mRenderScene->createRenderObj(_j); });
		//mGameScene.load(mSceneName);
		mGameWorld.init();
		mLoader.loadScene(mSceneName, mGameWorld);
	}

	~VRSample() {
	}

	std::string		mSceneName;

	// World & world layers
	World			mGameWorld;
	RenderLayer*	mRenderScene;

	// Common components
	ForwardRenderer mRenderer;

private:
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		if (_argc > 1) {
			mSceneName = _argv[1];
		}
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {
		return mGameWorld.update(_dt);
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	VRSample app(_argc,_argv);

	while(app.update());
	return 0;
}