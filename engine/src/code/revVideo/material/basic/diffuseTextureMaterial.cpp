////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 24th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// diffuse texture material

#include "diffuseTextureMaterial.h"

#include "revCore/resourceManager/resourceManager.h"
#include "revVideo/renderer/renderer3d.h"
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
	void CDiffuseTextureMaterial::setEnvironment() const
	{
		IVideoDriver * driver = SVideo::get()->driver();
		// Texture
		int textureUniformId = driver->getUniformId("texture");
		driver->setUniform(textureUniformId, 0, mTexture); // Set my texture into texture slot 0
		// Light
		int lightUniformId = driver->getUniformId("light0Dir");
		driver->setUniform(lightUniformId, CVec3(2.5f, -2.f, -3.f).normalize());
		// ViewPos
		IRenderer3d * renderer = SVideo::get()->renderer();
		int viewDirUniformId = driver->getUniformId("viewPos");
		driver->setUniform(viewDirUniformId, (renderer->viewMatrix() * CVec3::zero));
	}

}	// namespace video
}	// namespace rev