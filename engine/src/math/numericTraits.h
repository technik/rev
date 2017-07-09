//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple numeric traits

#ifndef _REV_MATH_NUMERIC_TRAITS_H_
#define _REV_MATH_NUMERIC_TRAITS_H_

namespace rev {
	namespace math {
		//----------------------------------------------
		template<typename T_>
		struct NumericTraits {
			static T_ one	() 	{ return 1;}
			static T_ zero	()	{ return 0;}
		};

		//----------------------------------------------
		template<>
		struct NumericTraits<float> {
			static float one	() 	{ return 1.f;}
			static float zero	()	{ return 0.f;}	
		};

		//----------------------------------------------
		template<>
		struct NumericTraits<double> {
			static double one	() 	{ return 1.0;}
			static double zero	()	{ return 0.0;}	
		};
	}
}

#endif // _REV_MATH_NUMERIC_TRAITS_H_