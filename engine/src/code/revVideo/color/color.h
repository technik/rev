////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on September 10th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Colors

#ifndef REV_REVVIDEO_COLOR_COLOR_H_
#define REV_REVVIDEO_COLOR_COLOR_H_

#include "revCore/types.h"

namespace rev { namespace video
{
	class CColor
	{
	public:
		// Constructors
		CColor(const TReal _r, const TReal _g, const TReal _b, const TReal _a = 1.f);

		// Accessors
		TReal	r	()	const;
		TReal	g	()	const;
		TReal	b	()	const;
		TReal	a	()	const;

	public:
		// Common colors
		static const CColor RED;
		static const CColor GREEN;
		static const CColor BLUE;

		static const CColor WHITE;
		static const CColor BLACK;

		static const CColor LIGHT_BLUE;

	private:
		TReal mR, mG, mB, mA;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Implementation
	//------------------------------------------------------------------------------------------------------------------
	inline CColor::CColor(const TReal _r, const TReal _g, const TReal _b, const TReal _a):
		mR(_r), mG(_g), mB(_b), mA(_a)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	inline TReal CColor::r() const	{	return mR;	}
	inline TReal CColor::g() const	{	return mG;	}
	inline TReal CColor::b() const	{	return mB;	}
	inline TReal CColor::a() const	{	return mA;	}

}	// namespace video
}	// namespace rev

#endif // REV_REVVIDEO_COLOR_COLOR_H_
