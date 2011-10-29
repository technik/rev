////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Material

#ifndef _REV_REVVIDEO_MATERIAL_MATERIAL_H_
#define _REV_REVVIDEO_MATERIAL_MATERIAL_H_

#include "revCore/codeTools/assert/assert.h"

namespace rev { namespace video
{
	// Forward declaration
	class CPxlShader;

	// Material base class
	class IMaterial
	{
	public:
		IMaterial():mShader(0){}
		~IMaterial()	{}
		CPxlShader*		shader			() const;	// Returns material's pixel shader
		virtual	void	setEnviroment	() const = 0;
	protected:
		CPxlShader * mShader;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline CPxlShader * IMaterial::shader() const
	{
		codeTools::revAssert(0 != mShader);
		return mShader;
	}
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_MATERIAL_MATERIAL_H_
