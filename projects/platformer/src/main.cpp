//----------------------------------------------------------------------------------------------------------------------
// Platformer game for testing purposes
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------

#include <iostream>

#include <engine.h>

#include "editor/editor.h"
#include "level/gameLevel.h"

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
	cam->move(Vec3f::zAxis());
	KeyboardInput* keyboard = KeyboardInput::get();

	platformer::GameLevel* level = new platformer::GameLevel;
	platformer::Editor* editor = new platformer::Editor(level);

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
		editor->update(frameTime);
		

		renderer->startFrame();
		renderer->setCamera(*cam);
		level->render(renderer);
		editor->render(renderer);
		renderer->finishFrame();
	}
	return 0;
}