//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// Minecraft-style sample game
//----------------------------------------------------------------------------------------------------------------------
#include "renderer.h"
#include "world.h"

#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>
#include <graphics/backend/pipeline.h>
#include <graphics/backend/renderPass.h>

#include <string>

using namespace rev::gfx;
using namespace rev::math;

namespace vkft::gfx
{
	//------------------------------------------------------------------------------------------------------------------
	Renderer::Renderer(rev::gfx::DeviceOpenGLWindows& device, const Vec2u& targetSize)
		: mGfxDevice(device)
	{
		initForwardPass(targetSize);
		initGeometryPipeline();
		mTargetFov = float(targetSize.x()) / targetSize.y();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::render(const World& world, const rev::gfx::Camera& camera)
	{
		mRenderCommands.clear();
		mRenderCommands.setPipeline(mFwdPipeline);

		mRenderCommands.setVertexData(world.quad.getVao());
		mRenderCommands.drawTriangles(world.quad.indices().count, CommandBuffer::IndexType::U16);

		mGfxDevice.renderQueue().submitPass(*mForwardPass);
		mGfxDevice.renderQueue().present();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::onResizeTarget(const rev::math::Vec2u& newSize)
	{
		mForwardPass->setViewport(Vec2u::zero(), newSize);
		mTargetFov = float(newSize.x()) / newSize.y();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::initForwardPass(const Vec2u& targetSize)
	{
		RenderPass::Descriptor fwdDesc;
		float grey = 0.5f;
		fwdDesc.clearColor = { grey,grey,grey, 1.f };
		fwdDesc.clearFlags = RenderPass::Descriptor::Clear::Color;
		fwdDesc.target = mGfxDevice.defaultFrameBuffer();

		// Allocate the render pass in the device
		mForwardPass = mGfxDevice.createRenderPass(fwdDesc);
		mForwardPass->setViewport(Vec2u::zero(), targetSize);
		mForwardPass->record(mRenderCommands);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::initGeometryPipeline()
	{
		// Create vertex shader
		const std::string vtxShaderCode = R"(
layout(location = 0) in vec3 vertex;

void main ( void )
{
	gl_Position = vec4(vertex.xy, 0.0, 1.0);
}
)";
		Pipeline::ShaderModule::Descriptor vtxDesc;
		vtxDesc.code = { vtxShaderCode };
		vtxDesc.stage = Pipeline::ShaderModule::Descriptor::Vertex;

		auto vtxShader = mGfxDevice.createShaderModule(vtxDesc);
		if(vtxShader.id == Pipeline::InvalidId)
			return;

		// Create pixel shader
		const std::string pxlShaderCode = R"(
out lowp vec3 outColor;

layout(location = 0) uniform vec3 color;

void main (void) {	
	outColor = color;
}
)";
		Pipeline::ShaderModule::Descriptor pxlDesc;
		pxlDesc.code = { pxlShaderCode };
		pxlDesc.stage = Pipeline::ShaderModule::Descriptor::Pixel;

		auto pxlShader = mGfxDevice.createShaderModule(pxlDesc);
		if(pxlShader.id == Pipeline::InvalidId)
			return;

		// Create the pipeline
		Pipeline::Descriptor pipelineDesc;
		pipelineDesc.vtxShader = vtxShader;
		pipelineDesc.pxlShader = pxlShader;
		mFwdPipeline = mGfxDevice.createPipeline(pipelineDesc);
	}
}