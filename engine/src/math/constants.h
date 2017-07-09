//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
//----------------------------------------------------------------------------------------------------------------------
// Common numeric constants
#pragma once

namespace rev {
	namespace math {

		template<typename T_> struct Constant  {
			static constexpr T_ Pi		= T_(Constant<long double>::Pi);
			static constexpr T_ TwoPi	= T_(2.0 * Constant<long double>::Pi);
			static constexpr T_ InvPi	= T_(1.0 / Constant<long double>::Pi);
			static constexpr T_ Inv2Pi	= T_(0.5 / Constant<long double>::Pi);
		};

		template<>
		struct Constant<long double> {
			static constexpr double Pi = 3.14159265358979323846L;
		};

	}
}