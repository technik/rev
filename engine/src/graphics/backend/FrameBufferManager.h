//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
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

#include "Vulkan/renderContextVulkan.h"

#include <map>
#include <vector>

namespace rev::gfx
{
	// Map from an ordered set of render targets to a frame buffer
	class FrameBufferManager
	{
	public:
		FrameBufferManager(vk::Device device)
			: m_device(device)
		{}

		vk::Framebuffer get(const std::vector<vk::ImageView>& targets, vk::RenderPass renderPass, const math::Vec2u& size)
		{
			auto key = computeTargetsKey(targets);
			auto iter = m_frameBuffers.find(key);

			if (iter == m_frameBuffers.end())
			{
				// Create a new frame buffer
				auto fbInfo = vk::FramebufferCreateInfo({},
					renderPass,
					targets,
					(uint32_t)size.x(), (uint32_t)size.y(), 1);

				auto newFrameBuffer = m_device.createFramebuffer(fbInfo);
				m_frameBuffers.insert({ key, newFrameBuffer });

				return newFrameBuffer;
			}
			else
			{
				return iter->second;
			}
		}

		// Discard existing frame buffers
		void clear()
		{
			m_knownTargets.clear();
			m_frameBuffers.clear();
		}

	private:
		vk::Device m_device;

		uint64_t computeTargetsKey(const std::vector<vk::ImageView>& targets)
		{
			uint64_t key = 0;

			for (auto& target : targets)
			{
				auto targetKey = getTargetKey(target);
				key = (key << 8) | targetKey;
			}

			return key;
		}

		uint64_t getTargetKey(vk::ImageView target)
		{
			auto knownIter = std::find(m_knownTargets.begin(), m_knownTargets.end(), target);

			uint64_t targetPos = 0;
			if (knownIter == m_knownTargets.end())
			{
				m_knownTargets.push_back(target);

				assert(m_knownTargets.size() < 255 && "Exceeded max render target capacity. FrameBuffer keys will be invalid.");

				targetPos = m_knownTargets.size() - 1;
			}
			else
			{
				targetPos = knownIter - m_knownTargets.begin();
			}

			return targetPos + 1; // Avoid returning 0 as a key, which would be confused with "no target";

		}

		std::vector<vk::ImageView> m_knownTargets;
		std::map<uint64_t, vk::Framebuffer> m_frameBuffers;
	};
}