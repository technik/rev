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

#include <math/algebra/vector.h>
#include <math/algebra/quaternion.h>
#include <memory>
#include <vector>

namespace rev::graphics {

	class Pose
	{
	public:
		struct JointPose
		{
			math::Quatf rotation;
			math::Vec3f translation;
			math::Vec3f scale;
		};

		std::vector<JointPose> joints;
	};

	class Animation
	{
	public:
		template<class T>
		struct Channel
		{
			std::vector<float> t;
			std::vector<T> values;
		};

		void getPose(float t, Pose& dst) const;
		float duration() const;

		std::vector<Channel<math::Vec3f>> m_translationChannels;
		std::vector<Channel<math::Quatf>> m_rotationChannels;
	};

}
