////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on November 4th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Solid color material

#ifndef _REV_REVVIDEO_MATERIAL_BASIC_RAYMARCHINGMATERIAL_H_
#define _REV_REVVIDEO_MATERIAL_BASIC_RAYMARCHINGMATERIAL_H_

#include "revVideo/material/material.h"
#include "revCore/types.h"

namespace rev { namespace video
{
	class CRaymarchingMaterial : public IMaterial
	{
	public:
		CRaymarchingMaterial();
		~CRaymarchingMaterial();

		void	setEnviroment () const;
		void	resetTime();
	private:
		mutable TReal mTime;
		mutable TReal mProf;
		mutable TReal mFrames;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline void CRaymarchingMaterial::resetTime()
	{
		mTime = 0.f;
	}

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_MATERIAL_BASIC_RAYMARCHINGMATERIAL_H_
