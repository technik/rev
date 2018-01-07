//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "../component.h"
#include <math/algebra/affineTransform.h>

namespace rev { namespace game {

	class Transform : public Component
	{
	public:
		math::AffineTransform xForm;

		math::Mat34f&			matrix() { return xForm.matrix(); }
		const math::Mat34f&		matrix() const { return xForm.matrix(); }
	};

} }	// namespace rev::game
