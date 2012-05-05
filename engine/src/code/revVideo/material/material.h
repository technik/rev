////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Material

#ifndef _REV_REVVIDEO_MATERIAL_MATERIAL_H_
#define _REV_REVVIDEO_MATERIAL_MATERIAL_H_

#include "revCore/codeTools/assert/assert.h"
#include "revCore/string.h"
#include "revCore/resourceManager/managedResource.h"
#include "revVideo/material/materialInstance.h"

namespace rev { namespace video
{
	// Forward declaration
	class CPxlShader;

	// Material base class
	class IMaterial: public IManagedResource<IMaterial,const char*,false>
	{
	public:
		IMaterial():mShader(0){}
		~IMaterial()	{}
		CPxlShader*		shader			() const;	// Returns material's pixel shader
		virtual bool	usesAlpha		() const { return false; }
		virtual	void	setEnvironment	() const = 0;

		virtual CMaterialInstance * getInstance () { return new CMaterialInstance(this); }

	protected:
		CPxlShader * mShader;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------
	inline CPxlShader * IMaterial::shader() const
	{
		revAssert(0 != mShader, "Error: shader doesn't exist");
		return mShader;
	}
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_MATERIAL_MATERIAL_H_
