//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// on 2014-10-29
//----------------------------------------------------------------------------------------------------------------------
// Simple numeric traits

#ifndef _REV_MATH_NUMERIC_TRAITS_H_
#define _REV_MATH_NUMERIC_TRAITS_H_

namespace rev {
	namespace math {
		// Values of math constants taken from
		// http://www.exploringbinary.com/pi-and-e-in-binary/
		// or computed as the binary approximation, then translated to decimal.
		// Which is more precise than rounding in decimal

		//----------------------------------------------
		template<typename T_>
		struct Constants {
		};

		//----------------------------------------------
		template<>
		struct Constants<float> {
			static constexpr float e		= 2.71828174591064453125f;
			static constexpr float pi		= 3.1415927410125732421875f;
			static constexpr float twoPi	= 6.2831852436065673828125f;
			static constexpr float halfPi	= 1.57079637050628662109375f;
		};

		//----------------------------------------------
		template<>
		struct Constants<double> {
			static constexpr double e		= 2.718281828459045090795598298427648842334747314453125;
			static constexpr double pi		= 3.141592653589793115997963468544185161590576171875;
			static constexpr double twoPi	= 6.283185307179586676085136787150986492633819580078125;
			static constexpr double halfPi	= 1.5707963267948965579989817342720925807952880859375;
		};
	}
}

#endif // _REV_MATH_NUMERIC_TRAITS_H_