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
#include <graphics/scene/animation/animation.h>
#include <game/scene/transform/transform.h>
#include <game/scene/sceneNode.h>
#include <memory>

namespace rev::game {

	class Skeleton
	{
	public:
		Skeleton(const std::vector<std::shared_ptr<SceneNode>>& nodes)
		{
			m_jointNodes = nodes;
			for(auto n : nodes)
			{
				gfx::Pose::JointPose nodePose;
				nodePose.scale = math::Vec3f::ones();
				auto transform = n->component<Transform>();
				nodePose.rotation = transform->xForm.rotation();
				nodePose.translation = transform->xForm.position();
			}
		}

		void setPose(const gfx::Pose& pose)
		{
			int n = 0;
			for(auto& joint : pose.joints)
			{
				auto& node = m_jointNodes[n++];
				auto transform = node->component<Transform>();
				transform->xForm.position() = joint.translation;
				transform->xForm.setRotation(joint.rotation);
			}
		}
		void getReferencePose(gfx::Pose& dst) const { dst = m_referencePose; }

	private:
		gfx::Pose m_referencePose;

		std::vector<std::shared_ptr<SceneNode>> m_jointNodes;
	};

}
