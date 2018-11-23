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

#include <graphics/backend/device.h>
#include <graphics/backend/frameBuffer.h>
#include <graphics/backend/commandBuffer.h>
#include <graphics/scene/renderGeom.h>
#include <graphics/shaders/shaderCodeFragment.h>

namespace rev::gfx {

	class FullScreenPass
	{
	public:
		FullScreenPass(gfx::Device&, const char* code = nullptr);

		void setPassCode(const char* code);

		void render(const CommandBuffer::UniformBucket& passUniforms, CommandBuffer& out);

		bool isOk() const { return m_pipeline.isValid(); }

	private:
		gfx::Device& m_device;
		RenderGeom m_quad;

		ShaderCodeFragment m_baseCode;
		ShaderCodeFragment m_passCode;
		ShaderCodeFragment m_completeCode;
		Pipeline m_pipeline;
	};

}