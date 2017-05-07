//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Sample racing game

#include <util/app/app3d.h>
#include <core/components/sceneNode.h>
#include <game/geometry/procedural/basic.h>
#include <video/basicTypes/camera.h>
#include <video/graphics/renderer/material.h>
#include <video/graphics/renderer/renderObj.h>

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

		// Create basic game objects
		Material defMaterial;
		defMaterial.shader = Shader::manager()->get("red");
		RenderMesh* cube = Procedural::box(Vec3f(1.f));
		RenderObj cubeObj(cube);
		cubeObj.mMaterial = &defMaterial;
		// -- Camera --
		camera.addComponent(new Camera(1.5f, 0.1f, 1000.f));
		camera.setPos(Vec3f(0.f, -1.f, 0.f));
		world.addComponent(&cubeObj);
		world.setPos(Vec3f(0.f, 1.f, 0.f));
	}

	~VRSample() {
	}

	SceneNode camera;
	SceneNode world;
	SceneNode light;

private:
	
	//----------------------------------------------------------------
	void processArgs(int _argc, const char** _argv) {
		_argc;
		_argv;
	}

	//----------------------------------------------------------------
	bool frame(float _dt) override {
		return true;
	}
};

// ---- Generic main loop ----
int main(int _argc, const char** _argv) {
	
	VRSample app(_argc,_argv);

	while(app.update());
	return 0;
}