////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 24th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// diffuse texture material

#include "diffuseTextureMaterial.h"

#include "revVideo/texture/texture.h"
#include "revVideo/video.h"
#include "revVideo/videoDriver/shader/pxlShader.h"
#include "revVideo/videoDriver/videoDriver.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CDiffuseTextureMaterial::CDiffuseTextureMaterial(const char * _textureName)
	{
		mShader = CPxlShader::manager()->get("perPixelDiffuse.pxl");
		mTexture = CTexture::manager()->get(_textureName);
	}

	//------------------------------------------------------------------------------------------------------------------
	CDiffuseTextureMaterial::~CDiffuseTextureMaterial()
	{
		CPxlShader::manager()->release(mShader);
		CTexture::manager()->release(mTexture);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDiffuseTextureMaterial::setEnviroment() const
	{
		IVideoDriver * driver = SVideo::get()->driver();
		int textureUniformId = driver->getUniformId("texture");
		driver->setUniform(textureUniformId, 0, mTexture); // Set my texture into texture slot 0
	}

}	// namespace video
}	// namespace rev