//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
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

	//------------------------------------------------------------------------------------------------------------------
	template<>
	class NumericTraits<int>
	{
	public:
#ifndef WIN32
		static constexpr int	zero() { return 0; }
#else // WIN32
		static int			zero() { return 0; }
#endif // WIN32
	};

	//------------------------------------------------------------------------------------------------------------------
	template<>
	class NumericTraits<unsigned>
	{
	public:
#ifndef WIN32
		static constexpr unsigned	zero() { return 0; }
#else // WIN32
		static unsigned				zero() { return 0; }
#endif // WIN32
	};

}	// namespace math
}	// namespace rev

#endif // _REV_CORE_MATH_NUMERICTRAITS_H_
