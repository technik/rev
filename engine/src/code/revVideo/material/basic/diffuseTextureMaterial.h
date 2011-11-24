////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 24th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// diffuse texture material

#ifndef _REV_REVVIDEO_MATERIAL_BASIC_DIFFUSETEXTUREMATERIAL_H_
#define _REV_REVVIDEO_MATERIAL_BASIC_DIFFUSETEXTUREMATERIAL_H_

#include "revVideo/material/material.h"

namespace rev { namespace video
{
	// Forward declarations
	class CTexture;
	class CPxlShader;

	class CDiffuseTextureMaterial : public IMaterial
	{
	public:
		CDiffuseTextureMaterial(const char * _textureName);
		~CDiffuseTextureMaterial();

	private:
		void setEnviroment() const;

	private:
		CTexture*	mTexture;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_MATERIAL_BASIC_DIFFUSETEXTUREMATERIAL_H_
