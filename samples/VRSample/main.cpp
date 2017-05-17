//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Sample racing game

#include <util/app/app3d.h>
#include <iostream>
#include <game/scene/scene.h>
#include <video/basicTypes/camera.h>
#include <video/graphics/renderObj.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/staticRenderMesh.h>
#include <cjson/json.h>

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
		// Component factories
		mGameScene.registerFactory("RenderObj", [](const cjson::Json& _data) {
			string fileName = _data["file"];
			return new RenderObj(StaticRenderMesh::loadFromFile(fileName));
		});
		mGameScene.registerFactory("Camera", [](const cjson::Json& _data) {
			float fov = (float)_data["fov"];
			float nearPlane = (float)_data["near"];
			float farPlane = (float)_data["far"];
			return new Camera(fov, nearPlane, farPlane);
		});
		mGameScene.load(mSceneName);

		// --- Create basic game objects
		// Camera
		//cam = new Camera(1.5f, 0.1f, 1000.f);
		//camera.addComponent(cam);
		//camera.setPos(Vec3f(0.f, -3.f, 0.f));
		//// World
		//StaticRenderMesh* cube = StaticRenderMesh::loadFromFile("data/unitSphere.rmd");
		//RenderObj* cubeObj = new RenderObj(cube);
		//renderScene.objects.push_back(cubeObj);
		//world.addComponent(cubeObj);
		//world.setPos(Vec3f(0.f, 2.f, 0.f));
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