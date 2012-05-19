////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 30th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Simple renderable quads

#include "quad.h"

#include <revCore/math/vector.h>
#include <revVideo/texture/texture.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/shader/vtxShader.h>
#include <revVideo/videoDriver/videoDriver.h>

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CQuad::CQuad(const CVec2& _size)
	{
		// Request the shader
		mShader = CVtxShader::get("guiPanel.vtx");
		// Allocate space for data
		mVertices = new float[12]; // 3 components x 4 vertices
		mUVs = new float[8]; // 2 components x 4 vertices
		mIndices = new unsigned short[4]; // 4 indices (we draw the quad as a triangle strip)
		// Fill the data
		// Indices
		mIndices[0] = 1;
		mIndices[1] = 0;
		mIndices[2] = 2;
		mIndices[3] = 3;
		// Vertices
		mVertices[2] = 0.f;
		mVertices[5] = 0.f;
		mVertices[8] = 0.f;
		mVertices[11] = 0.f;
		setSize(_size);
		// UVs
		mUVs[0] = 0.f;
		mUVs[1] = 0.f;
		mUVs[2] = 0.f;
		mUVs[3] = 1.f;
		mUVs[4] = 1.f;
		mUVs[5] = 1.f;
		mUVs[6] = 1.f;
		mUVs[7] = 0.f;
	}

	//------------------------------------------------------------------------------------------------------------------
	CQuad::~CQuad()
	{
		delete mIndices;
		delete mVertices;
		delete mUVs;
		CVtxShader::release(mShader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CQuad::setSize(const CVec2& _size)
	{
		CVec2 halfSize = _size * 0.5;
		mVertices[0] = -halfSize.x;
		mVertices[1] = -halfSize.y;
		mVertices[3] = -halfSize.x;
		mVertices[4] = halfSize.y;
		mVertices[6] = halfSize.x;
		mVertices[7] = halfSize.y;
		mVertices[9] = halfSize.x;
		mVertices[10] = -halfSize.y;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CQuad::setEnvironment() const
	{
		IVideoDriver * driver = SVideo::get()->driver();
		driver->setRealAttribBuffer(IVideoDriver::eVertex, 4, 3, mVertices);
		driver->setRealAttribBuffer(IVideoDriver::eNormal, 0, 3, 0);
		driver->setRealAttribBuffer(IVideoDriver::eTexCoord, 4, 2, mUVs);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CQuad::render() const
	{
		SVideo::get()->driver()->drawIndexBuffer(4, mIndices, IVideoDriver::eTriStrip);
	}

}	// namespace video
}	// namespace rev