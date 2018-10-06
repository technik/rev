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
#include <vector>
#include <game/scene/component.h>
#include <game/scene/sceneNode.h>
#include <game/scene/transform/transform.h>
#include <graphics/scene/animation/animation.h>
#include <memory>

namespace rev::game {

	class Animator : public Component
	{
	public:
		void pause();
		void resume();
		void reset();

		void playAnimation(std::shared_ptr<gfx::Animation>& anim, bool loop)
		{
			m_anim = anim;
			m_time = 0;
			m_loop = loop;
		}

		void init() override
		{
			m_time = 0;
			m_tempPose.scale = math::Vec3f::ones();

			m_target = node()->component<Transform>();
		}

		void update(float _dt) override {
			m_time += _dt;
			auto duration = m_anim->duration();
			if(m_loop)
			{
				while(m_time > duration)
					m_time -= duration;
			}

			m_anim->getChannelPose(0, m_time, m_tempPose);

			m_target->xForm.position() = m_tempPose.translation;
			m_target->xForm.setRotation(m_tempPose.rotation);
		}

	private:
		gfx::Pose::JointPose m_tempPose;
		Transform* m_target = nullptr;
		float m_time;
		bool m_loop = false;
		std::shared_ptr<gfx::Animation> m_anim;
	};

}
