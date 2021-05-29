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
#include "transform/transform.h"
#include "sceneNode.h"
#include <graphics/scene/Light.h>
#include <graphics/scene/renderScene.h>
#include <memory>
#include <cmath>

namespace rev { namespace game {

	class SpotLight : public Component
	{
	public:
		SpotLight(gfx::RenderScene& _scene, float apertureRadians, float range, const math::Vec3f& color)
		{
			mLight = std::make_shared<gfx::SpotLight>();
			_scene.addLight(mLight);
			mLight->range = range;
			mLight->color = color;
			mLight->maxCosine = cos(apertureRadians/2);
		}

		void init() override
		{
			mTransform = node()->component<Transform>();
		}

		void update(float) override
		{
			mLight->position = mTransform->absoluteXForm().position();
		}

	private:
		Transform* mTransform = nullptr;
		std::shared_ptr<gfx::SpotLight>	mLight;
	};

}}	// namespace rev::game