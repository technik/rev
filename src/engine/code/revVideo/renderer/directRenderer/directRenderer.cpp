////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// direct renderer

#include "directRenderer.h"

// TODO: Remove me
#include "revVideo/color/color.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CDirectRenderer::CDirectRenderer()
	{
		IVideoDriver * driver = SVideo::get()->driver();
		driver->setBackgroundColor(CColor::BLACK);
		mShader = driver->getShader("direct.vtx", "direct.pxl");

		mVertices[0] = 1.f;
		mVertices[1] = -1.f;
		mVertices[2] = 0.f;
		mVertices[3] = 1.f;
		mVertices[4] = -1.f;
		mVertices[5] = -1.f;

		mIndices[0] = 0;
		mIndices[1] = 1;
		mIndices[2] = 2;
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
		driver->drawIndexBuffer(3, mIndices, false);
	}
}	// namespace video
}	// namespace rev
