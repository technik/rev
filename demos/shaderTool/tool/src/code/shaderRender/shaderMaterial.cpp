////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, Shader tool
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 12th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shader material

#include "shaderMaterial.h"

#include <revCore/resourceManager/resourceManager.h>
#include <revCore/time/time.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/shader/pxlShader.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev;
using namespace rev::video;

//----------------------------------------------------------------------------------------------------------------------
ShaderMaterial::ShaderMaterial(const char * _pixelShader)
	:mTime(0.f)
{
	mShader = CPxlShader::manager()->get(_pixelShader);
}

//----------------------------------------------------------------------------------------------------------------------
ShaderMaterial::~ShaderMaterial()
{
	CPxlShader::manager()->release(mShader);
}

//----------------------------------------------------------------------------------------------------------------------
void ShaderMaterial::setEnvironment() const
{
	float dt = STime::get()->frameTime();
	mTime +=dt;
	IVideoDriver * driver = SVideo::get()->driver();
	int uniformId = driver->getUniformId("time");
	driver->setUniform(uniformId, mTime);
}