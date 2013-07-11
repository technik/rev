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

#include <revCore/time/time.h>
#include <revGraphics3d/renderable/renderScene.h>
#include <revGraphics3d/renderer/forward/forwardRenderer.h>
#include <revInput/keyboard/keyboardInput.h>
#include <revInput/keyboard/windows/keyboardInputWindows.h>
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/types/color/color.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev::input;
using namespace rev::math;
using namespace rev::video;

namespace rev { namespace graphics3d {

	//------------------------------------------------------------------------------------------------------------------
	Application3d::Application3d()
	{
		Time::init();
		KeyboardInput::init();
		VideoDriver::startUp();
		mVideoDriver = VideoDriver::get();
		mVideoDriver->createMainWindow();
		mVideoDriver->init3d();
		mDriver3d = VideoDriver::getDriver3d();
		mDriver3d->setClearColor(Color::Color(0.2f));
		mRenderer = new ForwardRenderer;

		Time::get()->update();
		mCamPos = Vec3f(0.f, 0.f, 0.f);
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
				KeyboardInputWindows * keyboard = static_cast<KeyboardInputWindows*>(KeyboardInput::get());
				keyboard->processWindowsMessage(msg);
				if(msg.message==WM_QUIT)
					return;
				else {
					TranslateMessage(&msg);	// Translate The Message
					DispatchMessage(&msg);
				}
			}
#endif // _WIN32
			KeyboardInput::get()->refresh();
			if(!update()) return;
			mDriver3d->clearColorBuffer();
			mDriver3d->clearZBuffer();
			mDriver3d->setViewport(Vec2i::zero(), Vec2u(640, 480));
			moveCamera();
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

	//------------------------------------------------------------------------------------------------------------------
	void Application3d::moveCamera()
	{
		float dt = Time::get()->frameTime();
		KeyboardInput* input = KeyboardInput::get();
		if(input->held(KeyboardInput::eA))	mCamPos.x -= dt;
		if(input->held(KeyboardInput::eD))	mCamPos.x += dt;
		Mat34f camView = Mat34f::identity();
		camView[0][3]= mCamPos.x;
		mCamera.setView(camView);
	}

}	// namespace graphics3d
}	// namespace rev
