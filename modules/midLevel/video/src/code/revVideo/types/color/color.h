//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 10th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Color

#ifndef _REV_VIDEO_TYPES_COLOR_H_
#define _REV_VIDEO_TYPES_COLOR_H_

namespace rev { namespace video
{
	class Color
	{
	public:
		Color(float _i);
		Color(float _r, float _g, float _b);
		Color(float _r, float _g, float _b, float _a);
	public:
		float r, g, b, a;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline Color::Color(float _i)
		:r(_i)
		,g(_i)
		,b(_i)
		,a(1.f)
		{}

	//------------------------------------------------------------------------------------------------------------------
	inline Color::Color(float _r, float _g, float _b)
		:r(_r)
		,g(_g)
		,b(_b)
		,a(1.f)
		{}

	//------------------------------------------------------------------------------------------------------------------
	inline Color::Color(float _r, float _g, float _b, float _a)
		:r(_r)
		,g(_g)
		,b(_b)
		,a(_a)
		{}

}	// namespace video
}	// namespace rev

#endif // _REV_VIDEO_TYPES_COLOR_H_
