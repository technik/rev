//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <cmath>
#include <math/constants.h>

namespace rev {
	namespace math {
		
		/// First order Low pass linear filter
		template<typename T_>
		struct LinearLowPass1stOrderFilter {
			LinearLowPass1stOrderFilter(const T_& _y0, float cutOffFreq_, float dt_)
				: y(_y0)
				, Fc(cutOffFreq_)
				, Wc(Fc / (Constant<float>::TwoPi))
				, Cf(std::exp(-Wc*dt_))
			{}

			T_ y; ///< Internal state

			/// Parentheses operator applies this filter to an input signal _x
			/// \return the resulting internal state, which is the filtered signal y
			const T_& operator() (const T_& _x) {
				y = y * Cf + _x * (1.f - Cf);
				return y;
			}

			const float Fc; ///< Cut off frequency, in s^-1
			const float Wc; ///< Natural cut off frequency, in rad/s
			const float Cf; ///< Precomputed filter constant for the given sample period
		};

		template<typename T_>
		using LowPassFilter1st = LinearLowPass1stOrderFilter<T_>;
	}
}