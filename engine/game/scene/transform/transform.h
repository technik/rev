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

		math::Mat44f&			matrix() { return xForm.matrix(); }
		const math::Mat44f&		matrix() const { return xForm.matrix(); }

		Transform()// = default;
		{
			xForm.matrix().setIdentity();
			mAbsoluteXForm.matrix().setIdentity();
		}
		Transform(std::istream& _in)
		{
			_in.read((char*)xForm.matrix().data(), 12*sizeof(float));
		}

		void update(float _dt) override;
		const math::AffineTransform& absoluteXForm() const { return mAbsoluteXForm; }

	private:
		math::AffineTransform mAbsoluteXForm;
	};

} }	// namespace rev::game
