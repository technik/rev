////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on April 27th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// scripted material

#ifndef _REV_VIDEO_MATERIAL_BASIC_SCRIPTEDMATERIAL_H_
#define _REV_VIDEO_MATERIAL_BASIC_SCRIPTEDMATERIAL_H_

#include <revCore/string.h>
#include <revVideo/material/material.h>
#include <utility.h>
#include <vector.h>

namespace rev { namespace video
{
	class CScriptedMaterial : public IMaterial
	{
	public:
		CScriptedMaterial(const char * _scriptFileName);
		~CScriptedMaterial();

		bool	usesAlpha		() const { return mUseAlpha; }
		void	setEnvironment	() const;

	private:
		bool	mUseAlpha;
		rtl::vector<rtl::pair<int,string> >	mUniforms;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_MATERIAL_BASIC_SCRIPTEDMATERIAL_H_