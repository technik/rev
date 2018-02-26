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
		struct NumericTraits {
			static T_ one	() 	{ return 1;}
			static T_ zero	()	{ return 0;}
			static T_ e		()	{ return 3;}
			static T_ pi	()	{ return 3;}
			static T_ twoPi	()	{ return 6;}
			static T_ halfPi()	{ return 2;}
		};

		//----------------------------------------------
		template<>
		struct NumericTraits<float> {
			static float one	() 	{ return 1.f;}
			static float zero	()	{ return 0.f;}	
			static float e		()	{ return 2.71828174591064453125f;}
			static float pi		()	{ return 3.1415927410125732421875f;}
			static float twoPi	()	{ return 6.2831852436065673828125f;}
			static float halfPi ()	{ return 1.57079637050628662109375f;}
		};

		//----------------------------------------------
		template<>
		struct NumericTraits<double> {
			static double one	() 	{ return 1.0;}
			static double zero	()	{ return 0.0;}
			static double e		()	{ return 2.718281828459045090795598298427648842334747314453125;}
			static double pi	()	{ return 3.141592653589793115997963468544185161590576171875;}
			static double twoPi	()	{ return 6.283185307179586676085136787150986492633819580078125;}
			static double halfPi()	{ return 1.5707963267948965579989817342720925807952880859375;}
		};
	}
}

#endif // _REV_MATH_NUMERIC_TRAITS_H_