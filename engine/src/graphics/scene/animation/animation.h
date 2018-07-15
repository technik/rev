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
		float duration() const
		{
			return m_rotationChannels[0].t.back();
		}
		
		void getChannelPose(size_t channelNdx, float t, Pose::JointPose& dst)
		{
			auto& channel = m_rotationChannels[channelNdx];
			if(channel.t.empty())
				return;
			if(t <= channel.t[0])
			{
				dst.rotation = channel.values[0];
				return;
			}
			if(t >= channel.t.back())
			{
				dst.rotation = channel.values.back();
				return;
			}
			// Value is within two others
			for(int i = 0; i < channel.t.size(); ++i)
			{
				if(channel.t[i+1] > t)
				{
					auto& v0 = channel.values[i];
					auto& v1 = channel.values[i+1];
					auto t0 = channel.t[i];
					auto t1 = channel.t[i+1];
					auto f = (t-t0)/(t1-t0);
					auto v = math::Quatf::lerp(v0, v1, f);
					dst.rotation = v;
					break;
				}
			}
		}

		std::vector<Channel<math::Vec3f>> m_translationChannels;
		std::vector<Channel<math::Quatf>> m_rotationChannels;
	};

}
