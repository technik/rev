////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// direct renderer

#include "directRenderer.h"

// TODO: Remove me
#include "revVideo/camera/orthoCamera.h"
#include "revVideo/color/color.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"
#include "revVideo/viewport/viewport.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CDirectRenderer::CDirectRenderer()
	{
		IVideoDriver * driver = SVideo::get()->driver();
		driver->setBackgroundColor(CColor::BLACK);
		mShader = driver->getShader("direct.vtx", "direct.pxl");

		mMVP = CMat4::identity();

		mVertices[0] = 1.f;
		mVertices[1] = -1.f;
		mVertices[2] = 0.f;
		mVertices[3] = 1.f;
		mVertices[4] = -1.f;
		mVertices[5] = -1.f;

		mIndices[0] = 0;
		mIndices[1] = 1;
		mIndices[2] = 2;

		CViewport * v1 = new CViewport(CVec2(0.f, 0.f), CVec2(1.f, 1.0f), 0.f);
		COrthoCamera * cam1 = new COrthoCamera(CVec2(20.f, 20.f), -1.f, 1.f);
		v1->setCamera(cam1);
	}

	//------------------------------------------------------------------------------------------------------------------
	CDirectRenderer::~CDirectRenderer()
	{
		//
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDirectRenderer::renderFrame()
	{
		IVideoDriver * driver = SVideo::get()->driver();

		driver->setShader(mShader);
		driver->setRealAttribBuffer(IVideoDriver::eVertex, 2, mVertices);

		for(CViewport::TViewportContainer::iterator i = CViewport::viewports().begin();
			i != CViewport::viewports().end(); ++i)
		{
			CViewport * viewport = (*i).second;
			if(viewport->camera())
			{
				CVec2 position = (*i).second->pos();
				mMVP = viewport->camera()->projMatrix();
				driver->setUniform(IVideoDriver::eMVP, mMVP);
				driver->drawIndexBuffer(3, mIndices, false);
			}
		}

	}
}	// namespace video
}	// namespace rev
