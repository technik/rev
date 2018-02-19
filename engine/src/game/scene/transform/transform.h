//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include "../component.h"
#include <math/algebra/affineTransform.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/debug/imgui.h>

namespace rev { namespace game {

	class Transform : public Component
	{
	public:
		math::AffineTransform xForm;

		math::Mat34f&			matrix() { return xForm.matrix(); }
		const math::Mat34f&		matrix() const { return xForm.matrix(); }

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

		/*void showDebugInfo() override {
			graphics::gui::text("Transform");
			math::Vec3f pos = xForm.position();
			ImGui::InputFloat3("pos", pos.data());
			//gui::slider("angle", mCamAngle, -3.2f, 3.2f);
			graphics::gui::slider("x", pos.x(), -500.f, 500.f);
			graphics::gui::slider("y", pos.y(), -500.f, 500.f);
			graphics::gui::slider("z", pos.z(), 0.f, 600.f);
			xForm.position() = pos;
		}*/

		void serialize(std::ostream& _out) const override
		{
			_out << "Transform\n"; // Type, for factories
			_out.write((const char*)xForm.matrix().data(), 12*sizeof(float));
		}

	private:
		math::AffineTransform mAbsoluteXForm;
	};

} }	// namespace rev::game
