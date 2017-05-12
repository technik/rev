//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Sample racing game

#include <util/app/app3d.h>
#include <core/components/sceneNode.h>
#include <game/geometry/procedural/basic.h>
#include <video/basicTypes/camera.h>
#include <video/graphics/renderObj.h>
#include <video/graphics/renderScene.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderer/renderContext.h>
#include <video/window/window.h>

using namespace rev::core;
using namespace rev::game;
using namespace rev::input;
using namespace rev::math;
using namespace rev::video;

class VRSample : public rev::App3d {
public:
	VRSample(int _argc, const char** _argv)
		: App3d(_argc, _argv)
	{
		processArgs(_argc, _argv);
		mRenderer.init(&driver3d());

		// --- Create basic game objects
		// Camera
		cam = new Camera(1.5f, 0.1f, 1000.f);
		camera.addComponent(cam);
		camera.setPos(Vec3f(0.f, -10.f, 0.f));
		// World
		StaticRenderMesh* cube = Procedural::box(Vec3f(1.f));
		RenderObj* cubeObj = new RenderObj(cube);
		renderScene.objects.push_back(cubeObj);
		world.addComponent(cubeObj);
		world.setPos(Vec3f(0.f, 10.f, 0.f));
	}

	~VRSample() {
	}

	//RenderObj* cubeObj;
	Camera* cam;
	SceneNode camera;
	SceneNode world;
	RenderScene renderScene;
	ForwardRenderer mRenderer;


private:
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		_argc;
		_argv;
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {
		mRenderer.render(renderScene, *cam);
		return true;
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	VRSample app(_argc,_argv);

	while(app.update());
	return 0;
}