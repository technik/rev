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

/*#include "fullScreenPass.h"

namespace rev::gfx {

	FullScreenPass::FullScreenPass(gfx::Device& device, ShaderCodeFragment* code)
		: m_device(device)
	{
		// Create a full screen quad
		m_quad = rev::gfx::RenderGeom::quad({2.f, 2.f});

		// Common pass code
		m_baseCode = new ShaderCodeFragment(R"(
#ifdef VTX_SHADER
	layout(location = 0) in vec3 vertex;

	void main ( void )
	{
		gl_Position = vec4(vertex.xy, 1.0, 1.0);
	}
#endif

#ifdef PXL_SHADER
out lowp vec3 outColor;

vec3 shade();

void main (void) {	
	outColor = shade();
}
#endif
)");
		// Initialize pipeline on start
		if(code)
			setPassCode(code);
	}

	FullScreenPass::~FullScreenPass()
	{
		if(m_completeCode)
			delete m_completeCode;
		if(m_baseCode)
			delete m_baseCode;
		if(m_passCode)
			delete m_passCode;
	}

	void FullScreenPass::setPassCode(ShaderCodeFragment* code)
	{
		assert(code);
		m_pipeline = Pipeline(); // Invalidate pipeline

		if(code != m_passCode)
		{
			m_passCode = code;
			m_shaderListeners.push_back(code->onReload(
				[this](ShaderCodeFragment& reloadedCode) {
					this->setPassCode(&reloadedCode);
				})
			);
			m_completeCode = new ShaderCodeFragment(m_baseCode, m_passCode);
		}

		// Prepare code
		Pipeline::ShaderModule::Descriptor stageDesc;
		m_completeCode->collapse(stageDesc.code);

		// Vertex shader
		stageDesc.stage = Pipeline::ShaderModule::Descriptor::Vertex;
		auto vtxShader = m_device.createShaderModule(stageDesc);
		if(vtxShader.id == Pipeline::InvalidId)
			return;

		// Pixel shader
		stageDesc.stage = Pipeline::ShaderModule::Descriptor::Pixel;
		auto pxlShader = m_device.createShaderModule(stageDesc);
		if(pxlShader.id == Pipeline::InvalidId)
			return;

		// Link pipeline
		Pipeline::Descriptor pipelineDesc;
		pipelineDesc.vtxShader = vtxShader;
		pipelineDesc.pxlShader = pxlShader;
		pipelineDesc.raster.depthTest = Pipeline::DepthTest::Lequal;
		m_pipeline = m_device.createPipeline(pipelineDesc);
	}

	void FullScreenPass::render(const CommandBuffer::UniformBucket& passUniforms, CommandBuffer& out)
	{
		if(!m_pipeline.isValid())
			return;

		out.setPipeline(m_pipeline);
		out.setUniformData(passUniforms);
		out.setVertexData(m_quad.getVao());
		out.drawTriangles(6, CommandBuffer::IndexType::U16, nullptr);
	}

}
*/