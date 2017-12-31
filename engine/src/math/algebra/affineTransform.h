//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <math/algebra/matrix.h>
#include <math/algebra/quaternion.h>
#include <math/algebra/vector.h>

namespace rev { namespace math {

	struct AffineTransform : public Mat34f {
	public:
		auto position	() 			{ return col(3); }
		auto position	() const	{ return col(3); }

		auto rotation	()			{ return block<3,3>(0,0); }
		auto rotation	()	const	{ return block<3,3>(0,0); }

		void rotate		(const Mat33f& _rot);
	};

	//-------------------------------------------------------------------------------------------
	inline void AffineTransform::rotate(const Mat33f& _rot) {
		rotation() = _rot * rotation();
	}

} }	// namespace rev::math
