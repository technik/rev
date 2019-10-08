//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <algorithm>
#include <cassert>
#include "matrixBase.h"

namespace rev::math {

	template<typename T, size_t n, class Derived>
	struct VectorExpr
	{
		// Generic component accessor.
		T operator[](size_t i) const { return static_cast<const Derived&>(*this)[i]; }

		// Component accessor for when you know the component index at compile time.
		template<size_t i>
		T  getComponent() const
		{
			static_assert(i < n);
			auto& derived = static_cast<const Derived&>(*this);
			return derived.template getComponent<i>();
		}

		// Named accessors
		T	x()	const { return getComponent<0>(); }
		T	y()	const { return getComponent<1>(); }
		T	z()	const { return getComponent<2>(); }
		T	w()	const { return getComponent<3>(); }
	};

}