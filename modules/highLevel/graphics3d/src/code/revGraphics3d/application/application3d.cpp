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

#include <revVideo/driver3d/driver3d.h>
#include <revVideo/types/color/color.h>
#include <revVideo/types/shader/pixel/pxlShader.h>
#include <revVideo/types/shader/vertex/vtxShader.h>
#include <revVideo/types/shader/vertex/openGL21/vtxShaderOpenGL21.h>
#include <revVideo/types/shader/shader.h>
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

		// Shaders
		VtxShader* vtxShader = VtxShader::get("test.vtx");
		// ----- WARNING: Highly platform dependent code -----
		VtxShaderOpenGL21* vtx21 = static_cast<VtxShaderOpenGL21*>(vtxShader);
		vtx21->addAttribute("vertex");
		// ----- End of platform dependent code --------------
		mBasicShader = Shader::get(std::make_pair(vtxShader, PxlShader::get("test.pxl")));
		//mColorUniformId = mShader->getUniformLocation("color");
		
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
		mDriver3d->setShader(mBasicShader);
		Vec3f vertices[] = { Vec3f(-0.5f, -0.3f, 0.f ),
							 Vec3f(0.5f, -0.3f, 0.f ),
							 Vec3f(0.f, 0.4f, 0.f) };

		mDriver3d->setAttribBuffer(0, 3, vertices);
		uint16_t indices[] = { 0, 1, 2 };
		mDriver3d->drawIndexBuffer(3, indices, Driver3d::EPrimitiveType::triangles);
	}

}	// namespace graphics3d
}	// namespace rev
