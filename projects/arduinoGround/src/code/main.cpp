//----------------------------------------------------------------------------------------------------------------------
// Rev Terminal
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On January 15th, 2013
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <revCanvas/application/canvasApplication.h>
#include <revCanvas/canvas/canvas.h>
#include <revVideo/types/color/color.h>

using namespace rev::canvas;

class TestCanvasApp : public CanvasApplication
{
public:
	void draw();
};

int main (int //_argc
	, const char** //_argv
	)
{
	CanvasApplication* app = new TestCanvasApp();
	while(app->update())
	{}
	delete app;

	return 0;
}

void TestCanvasApp::draw()
{
	canvas()->setDrawColor(rev::video::Color(0.f, 1.f, 1.f, 1.f));
	canvas()->rect(rev::math::Vec2f(0.f, 0.f), rev::math::Vec2f(1.f, 0.5f));
}