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
#include <graphics/scene/camera.h>

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

		Mat44f wvp = camera.viewProj(mTargetFov);
		mUniforms.clear();
		mUniforms.addParam(0, wvp);

		mRenderCommands.setUniformData(mUniforms);
		mRenderCommands.setVertexData(world.mTileGeom.getVao());
		mRenderCommands.drawTriangles(world.mTileGeom.indices().count, CommandBuffer::IndexType::U16, nullptr);

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

// Uniforms
layout(location = 0) uniform mat4 wvp;

void main ( void )
{
	gl_Position = wvp * vec4(vertex.xy, 0.0, 1.0);
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

void main (void) {	
	outColor = vec3(1.0);
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