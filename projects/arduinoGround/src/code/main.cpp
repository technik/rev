//----------------------------------------------------------------------------------------------------------------------
// Rev Terminal
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On January 15th, 2013
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <revCanvas/application/canvasApplication.h>
#include <revCanvas/canvas/canvas.h>
#include <revCommunication/serial/serialPort.h>
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/types/color/color.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev::canvas;

class TestCanvasApp : public CanvasApplication
{
public:
	void draw();
};

#ifdef ANDROID
int android_entry_point(int,const char**);

void android_main(struct android_app*)
{
	// Make sure glue isn't stripped.
    app_dummy();
	
	android_entry_point(0, nullptr);
}

int android_entry_point
#else // !ANDROID
int main 
#endif // !ANDROID
	(int //_argc
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
	rev::video::VideoDriver::getDriver3d()->setClearColor(rev::video::Color(0.4f));
	canvas()->setDrawColor(rev::video::Color(0.f));
	canvas()->rect(rev::math::Vec2f(200.f, 50.f), rev::math::Vec2f(160.f, 90.f));
	canvas()->setDrawColor(rev::video::Color(1.f));
	canvas()->line(rev::math::Vec2f(200.f, 50.f), rev::math::Vec2f(360.f, 140.f));
}