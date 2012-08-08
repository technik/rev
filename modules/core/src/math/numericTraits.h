//----------------------------------------------------------------------------------------------------------------------
// Rose: Robot oriented simulation environment
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Numeric traits

#ifndef _REV_CORE_MATH_NUMERICTRAITS_H_
#define _REV_CORE_MATH_NUMERICTRAITS_H_

namespace rev { namespace math
{
	//------------------------------------------------------------------------------------------------------------------
	template<class Number_>
	class NumericTraits
	{
	public:
#ifndef WIN32
		static constexpr Number_	zero();
#else // WIN32
		static			 Number_	zero();
#endif // WIN32
	};

	//------------------------------------------------------------------------------------------------------------------
	template<>
	class NumericTraits<float>
	{
	public:
#ifndef WIN32
		static constexpr float	zero() { return 0.f; }
#else // WIN32
		static float			zero() { return 0.f; }
#endif // WIN32
	};

}	// namespace math
}	// namespace rev

#endif // _REV_CORE_MATH_NUMERICTRAITS_H_
