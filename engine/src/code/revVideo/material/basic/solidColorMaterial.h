////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Solid color material

#ifndef _REV_REVVIDEO_MATERIAL_BASIC_SOLIDCOLORMATERIAL_H_
#define _REV_REVVIDEO_MATERIAL_BASIC_SOLIDCOLORMATERIAL_H_

#include "revVideo/material/material.h"

#include "revVideo/color/color.h"

namespace rev { namespace video
{
	class CSolidColorMaterial: public IMaterial
	{
	public:
		// Constructor
		CSolidColorMaterial(const CColor& _color);
		~CSolidColorMaterial();

		// Accessor methods
		void setColor(const CColor& _color);
		const CColor& color () const;
	private:
		void setEnviroment() const;

	private:
		CColor	mColor;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline void CSolidColorMaterial::setColor(const CColor& _color)
	{
		mColor = _color;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline const CColor& CSolidColorMaterial::color() const
	{
		return mColor;
	}

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_MATERIAL_BASIC_SOLIDCOLORMATERIAL_H_
