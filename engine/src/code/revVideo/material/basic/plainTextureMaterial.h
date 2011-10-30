////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 30th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// plain texture material

#ifndef _REV_REVVIDEO_MATERIAL_BASIC_PLAINTEXTUREMATERIAL_H_
#define _REV_REVVIDEO_MATERIAL_BASIC_PLAINTEXTUREMATERIAL_H_

#include "revVideo/material/material.h"

namespace rev { namespace video
{
	// Forward declaration
	class CTexture;

	class CPlainTextureMaterial: public IMaterial
	{
	public:
		// Constructor & destructor
		CPlainTextureMaterial(CTexture * _texture = 0);
		~CPlainTextureMaterial();

		// Accessor methods
		void setTexture (CTexture * _texture);

	private:
		void setEnviroment () const;

	private:
		CTexture * mTexture;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_MATERIAL_BASIC_PLAINTEXTUREMATERIAL_H_
