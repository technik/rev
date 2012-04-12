////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, Shader tool
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 12th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Render surface

#include "surface.h"

#include <revCore/resourceManager/resourceManager.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/videoDriver.h>
#include <revVideo/videoDriver/shader/vtxShader.h>

using namespace rev;
using namespace rev::video;

//----------------------------------------------------------------------------------------------------------------------
ShaderSurface::ShaderSurface(const char * _vtxShader)
{
	mShader = CVtxShader::manager()->get(_vtxShader);
	mCoordinates[0] = CVec2(-1.f,-1.f);
	mCoordinates[1] = CVec2(-1.f,3.f);
	mCoordinates[2] = CVec2(3.f,-1.f);
	mIndices[0] = 0;
	mIndices[1] = 1;
	mIndices[2] = 2;
}

//----------------------------------------------------------------------------------------------------------------------
ShaderSurface::~ShaderSurface()
{
	CVtxShader::manager()->release(mShader);
}

//----------------------------------------------------------------------------------------------------------------------
CVtxShader* ShaderSurface::shader() const
{
	return mShader;
}

//----------------------------------------------------------------------------------------------------------------------
void ShaderSurface::setEnvironment() const
{
	SVideo::getDriver()->setRealAttribBuffer(0, 2, mCoordinates);
}

//----------------------------------------------------------------------------------------------------------------------
void ShaderSurface::render() const
{
	SVideo::getDriver()->drawIndexBuffer(3, mIndices);
}