//----------------------------------------------------------------------------------------------------------------------
// Rev Terminal
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On January 15th, 2013
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <revCanvas/application/canvasApplication.h>

using namespace rev::canvas;

int main (int //_argc
	, const char** //_argv
	)
{
	CanvasApplication* app = new CanvasApplication();
	while(app->update())
	{}
	delete app;

	return 0;
}