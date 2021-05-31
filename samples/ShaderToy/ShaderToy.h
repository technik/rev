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

#include <game/application/base3dApplication.h>
#include <gfx/backend/DescriptorSet.h>
#include <math/algebra/vector.h>

#include <random>

namespace rev
{
	namespace core {
		class FolderWatcher;
	}

	namespace gfx {
		class FrameBufferManager;
		class FullScreenPass;
		class RasterPipeline;
	}

	class ShaderToy : public game::Base3dApplication
	{
	public:
		ShaderToy();
		~ShaderToy();

		void getCommandLineOptions(core::CmdLineParser& options) override;

		std::string name() override { return "ShaderToy"; }

		bool init() override;
		bool updateLogic(TimeDelta dt) override;
		void render(TimeDelta dt) override;

		struct PushConstants
		{
			math::Vec4f time;
		} m_frameConstants;

	private:
		void loadNoiseTextures();
		std::string m_shaderFileName = "shaderToy.frag.spv";

		math::Vec4f m_timeVector;
		std::unique_ptr<gfx::FrameBufferManager> m_frameBuffers;

		gfx::DescriptorSetLayout m_descSetLayout;
		std::unique_ptr<gfx::FullScreenPass> m_fullScreenFilter;
		vk::Semaphore m_imageAvailableSemaphore;
		std::shared_ptr<gfx::RasterPipeline> m_rasterCode;
		std::unique_ptr<core::FolderWatcher> m_shaderWatcher;

		// Noise
		static constexpr unsigned NumBlueNoiseTextures = 64;
		std::vector<std::shared_ptr<gfx::Texture>> m_blueNoise;
		std::default_random_engine m_rng;
		std::uniform_int_distribution<unsigned> m_noisePermutations;
	};
}
