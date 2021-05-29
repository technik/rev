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

#include "component.h"
#include "sceneNode.h"
#include "transform/transform.h"
#include <graphics/scene/camera.h>

namespace rev { namespace game {

	class Camera : public Component
	{
	public:

		//------------------------------------------------------------------------------------------
		Camera()
		{
			mCam = std::make_shared<gfx::Camera>();
		}

		//------------------------------------------------------------------------------------------
		Camera(float fov, float _near, float _far)
		{
			mCam = std::make_shared<gfx::Camera>(fov, _near, _far);
		}

		//------------------------------------------------------------------------------------------
		void init() override {
			mTransform = node()->component<Transform>();
		}

		//------------------------------------------------------------------------------------------
		void update(float _dt) override {
			assert(mTransform);
			mCam->setWorldTransform(mTransform->absoluteXForm());
		}

		const auto& cam() const { return mCam; }
		auto& cam() { return mCam; }

	private:
		Transform*	mTransform = nullptr;
		std::shared_ptr<gfx::Camera> mCam;
	};

}}
