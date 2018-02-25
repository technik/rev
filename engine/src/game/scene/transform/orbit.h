//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once
#include "../component.h"
#include "../sceneNode.h"
#include <math/algebra/vector.h>
#include "transform.h"
#include <input/pointingInput.h>

namespace rev { namespace game {

	class Orbit : public Component
	{
	public:
		Orbit(const math::Vec2f& speeds, int buttonId = 0) : mTouchNdx(buttonId), mSpeed(speeds) {}

		void init		() override
		{
			mSrcTransform = node()->component<Transform>();
		}

		void update		(float _dt) override
		{
			auto pointingInput = input::PointingInput::get();
			if(pointingInput)
			{
				bool down = false;
#ifdef ANDROID
				// TODO: Support 1/2 fingers
				down = pointingInput->leftDown() &&
					(pointingInput->nTouches() == (mTouchNdx+1));
#else
				down = mTouchNdx?pointingInput->middleDown():pointingInput->leftDown();
#endif
				if(ImGui::GetIO().WantCaptureMouse)
					down = false;
				if(down)
				{
					if(wasDown)
					{
						auto displacement = pointingInput->touchPosition() - mClickStartPos;
						angles.x() = ang0.x() + displacement.x()*mSpeed.x();
						angles.y() = ang0.y() + displacement.y()*mSpeed.y();
					}
					else
					{
						mClickStartPos = pointingInput->touchPosition();
						ang0 = angles;
					}
					wasDown = true;

					// Recompute transform
					auto pan = math::Quatf({0.f,0.f,1.f}, angles.x());
					auto tilt = math::Quatf({1.f,0.f,0.f}, angles.y());
					mSrcTransform->xForm.setRotation(pan * tilt);
				}
				else wasDown = false;
			}
		}

		void serialize(std::ostream &) const override {
			// TODO
		}

	private:
		int mTouchNdx = 0;
		math::Vec2f ang0 = math::Vec2f::zero();
		math::Vec2f angles = math::Vec2f::zero();
		bool wasDown = false;
		math::Vec2i mClickStartPos;
		math::Vec2f mSpeed;
		Transform*	mSrcTransform;
	};

}}	// namespace rev::game
