//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Sample racing game

#include <util/app/app3d.h>
#include <iostream>
#include <game/scene/scene.h>
#include <video/basicTypes/camera.h>
#include <video/graphics/material.h>
#include <video/graphics/renderObj.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/staticRenderMesh.h>
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
		mGameScene.load(mSceneName);
	}

	~VRSample() {
	}

	//RenderObj* cubeObj;
	//Camera* cam;
	//SceneNode camera;
	//SceneNode world;
	//RenderScene renderScene;
	Scene			mGameScene;
	ForwardRenderer mRenderer;
	std::string		mSceneName;

	World			mGameWorld;

private:
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		if (_argc > 1) {
			mSceneName = _argv[1];
		}
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {
		//mRenderer.render(renderScene, *cam);
		if(!mGameScene.update(_dt))
			return false;
		mGameScene.render(mRenderer);
		return true;
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	VRSample app(_argc,_argv);

	while(app.update());
	return 0;
}