//----------------------------------------------------------------------------------------------------------------------
// Platformer game for testing purposes
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------

#include <iostream>

#include <engine.h>

#include <core/memory/newAllocator.h>
#include <core/time/time.h>
#include <game/scene/camera/flyByCamera.h>
#include <game/geometry/procedural/basic.h>
#include <game/scene/sceneGraph/sceneNode.h>
#include <input/keyboard/keyboardInput.h>
#include <math/noise.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderer/renderObj.h>

typedef rev::Engine<rev::core::NewAllocator>	Engine;

using namespace rev::core;
using namespace rev::game;
using namespace rev::input;
using namespace rev::math;
using namespace rev::video;

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	Engine engine(_argc, _argv);

	std::cout << "Start platformer session\n";

	ForwardRenderer*	renderer = engine.create<ForwardRenderer>();
	renderer->init(engine.mainWindow(), engine);
	FlyByCamera* cam = engine.create<FlyByCamera>(1.54f, 1.3333f, 0.01f, 1000.f);
	RenderObj* obj = Procedural::box(Vec3f(1.f));

	SceneNode objNode;
	KeyboardInput* keyboard = KeyboardInput::get();

	const int size = 200;
	float height[size][size];
	for (unsigned i = 0; i < size; ++i)
		for (unsigned j = 0; j < size; ++j)
			height[i][j] = 5 * SNoise::simplex(float(i) / 40, float(j) / 40) + 0.5f*SNoise::simplex(float(i), float(j));
	cam->move(Vec3f(0.f, 0.f, height[0][0] + 2.f));
	cam->rotate(Vec3f::zAxis(), -0.78f);

	std::cout << " --- Game loop ---\n";

	while (engine.update())
	{
		float frameTime = Time::get()->frameTime();
		std::cout << "Frame time: " << frameTime << "\n";
		if (keyboard->held(KeyboardInput::eShift))
			cam->setSpeed(1.f);
		else
			cam->setSpeed(0.4f);
		cam->update(frameTime);

		renderer->startFrame();
		renderer->setCamera(*cam);
		for (int i = 0; i < size; ++i){
			for (int j = 0; j < size; ++j) {
				float h = height[i][j];
				objNode.setPos(Vec3f(float(i), float(j), h));
				obj->transform = objNode.transform();
				renderer->renderObject(*obj);
			}
		}
		renderer->finishFrame();
	}
	return 0;
}