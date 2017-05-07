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
#include <video/window/window.h>
#include <vector>
#include <iostream>

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
	}

	~VRSample() {
	}
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