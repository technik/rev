//----------------------------------------------------------------------------------------------------------------------
// Platformer game for testing purposes
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------

#include <core/memory/newAllocator.h>
#include <engine.h>
#include <game/scene/camera/flyByCamera.h>
#include <game/geometry/procedural/basic.h>
#include <game/scene/sceneGraph/sceneNode.h>
#include <core/time/time.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>
#include <video/graphics/renderer/renderObj.h>

typedef rev::Engine<rev::core::NewAllocator>	Engine;

using namespace rev::core;
using namespace rev::game;
using namespace rev::video;

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	Engine engine(_argc, _argv);

	ForwardRenderer*	renderer = engine.create<ForwardRenderer>();
	renderer->init(engine.mainWindow(), engine);
	FlyByCamera* cam = engine.create<FlyByCamera>(1.54f, 0.75f, 0.01f, 1000.f);
	cam->setSpeed(0.2f);
	RenderObj* obj = Procedural::box(Vec3f(1.f));

	SceneNode objNode;

	while (engine.update())
	{
		float frameTime = Time::get()->frameTime();
		cam->update(frameTime);

		renderer->startFrame();
		renderer->setCamera(*cam);
		const int size = 7;
		for (int i = -size; i <= size; ++i){
			for (int j = -size; j <= size; ++j) {
				objNode.setPos(Vec3f(float(i), float(j), float(i*j/10)));
				obj->transform = objNode.transform();
				renderer->renderObject(*obj);
			}
		}
		renderer->finishFrame();
	}
	return 0;
}