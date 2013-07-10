//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// base application

#ifdef _WIN32
#include <Windows.h>
#endif // _WIN32

#include "application3d.h"

#include <revGraphics3d/renderable/renderScene.h>
#include <revGraphics3d/renderer/forward/forwardRenderer.h>
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/types/color/color.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev::video;
using namespace rev::math;

namespace rev { namespace graphics3d {

	//------------------------------------------------------------------------------------------------------------------
	Application3d::Application3d()
	{
		VideoDriver::startUp();
		mVideoDriver = VideoDriver::get();
		mVideoDriver->createMainWindow();
		mVideoDriver->init3d();
		mDriver3d = VideoDriver::getDriver3d();
		mDriver3d->setClearColor(Color::Color(0.2f));
		mRenderer = new ForwardRenderer;

		mCamera.setProjection(Mat44f::frustrum(1.6f, 1.333f, 0.125f, 10000.f));
	}

	//------------------------------------------------------------------------------------------------------------------
	Application3d::~Application3d()
	{
		VideoDriver::shutDown();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Application3d::run()
	{
		for(;;)
		{
#ifdef _WIN32
			// TODO: Move this to a platform specific handler that can process and dispatch messages
			MSG msg;
			while(PeekMessage(&msg,NULL,0,0,PM_REMOVE))
			{
				if(msg.message==WM_QUIT)
					return;
				else {
					TranslateMessage(&msg);	// Translate The Message
					DispatchMessage(&msg);
				}
			}
#endif // _WIN32
			if(!update()) return;
			mDriver3d->clearColorBuffer();
			mDriver3d->clearZBuffer();
			mDriver3d->setViewport(Vec2i::zero(), Vec2u(640, 480));
			render();
			mDriver3d->finishFrame();
			if(!mVideoDriver->update())	return;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Application3d::render()
	{
		mRenderer->render(mCamera, *RenderScene::get());
	}

}	// namespace graphics3d
}	// namespace rev
