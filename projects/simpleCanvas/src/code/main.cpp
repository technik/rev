//----------------------------------------------------------------------------------------------------------------------
// Rev Generic template
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On October 23rd, 2012
//----------------------------------------------------------------------------------------------------------------------
// Entry point

#include <revCanvas/application/canvasApplication.h>

using namespace rev::canvas;

int main (int //_argc
	, const char** //_argv
	)
{
	CanvasApplication * canvasApp = new CanvasApplication();
	bool mustExit = false;
	while(!mustExit)
	{
		mustExit = !canvasApp->update();
	}
	return 0;
}