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
#ifndef _WIN32
		static constexpr Number_	zero();
		static constexpr Number_	one();
#else // WIN32
		static			 Number_	zero();
		static			 Number_	one();
#endif // WIN32
	};

	//------------------------------------------------------------------------------------------------------------------
	template<>
	class NumericTraits<int>
	{
	public:
#ifndef _WIN32
		static constexpr int	zero() { return 0; }
		static constexpr int	one () { return 1; }
#else // WIN32
		static int			zero() { return 0; }
		static int			one () { return 1; }
#endif // WIN32
	};

	//------------------------------------------------------------------------------------------------------------------
	template<>
	class NumericTraits<unsigned>
	{
	public:
#ifndef _WIN32
		static constexpr unsigned	zero() { return 0; }
		static constexpr unsigned	one () { return 1; }
#else // WIN32
		static unsigned				zero() { return 0; }
		static unsigned				one () { return 1; }
#endif // WIN32
	};

	//------------------------------------------------------------------------------------------------------------------
	template<>
	class NumericTraits<float>
	{
	public:
#ifndef _WIN32
		static constexpr float	zero() { return 0.f; }
		static constexpr float	one () { return 1.f; }
#else // WIN32
		static float			zero() { return 0.f; }
		static float			one () { return 1.f; }
#endif // WIN32
	};

}	// namespace math
}	// namespace rev

#endif // _REV_CORE_MATH_NUMERICTRAITS_H_
