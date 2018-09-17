//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/osHandler.h>
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/device.h>
#include <graphics/backend/Windows/windowsPlatform.h>
#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>
#include <graphics/scene/renderGeom.h>

#include <string>
#include <vector>

using namespace std;
using namespace rev::core;
using namespace rev::gfx;
using namespace rev::math;

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Init engine core systems
	OSHandler::startUp();
	FileSystem::init();

	// Create the application window
	Vec2u windowStart = {100, 150};
	Vec2u windowSize = { 200, 200 };
	auto wnd = createWindow(windowStart, windowSize, "Vulkraft", true);

	// Init graphics
	auto gfxDevice = DeviceOpenGLWindows(wnd, true);
	auto& renderQueue = gfxDevice.renderQueue();

	// Renderpass
	RenderPass::Descriptor fwdDesc;
	float grey = 0.5f;
	fwdDesc.clearColor = { grey,grey,grey, 1.f };
	fwdDesc.clearFlags = RenderPass::Descriptor::Clear::Color;
	fwdDesc.target = gfxDevice.defaultFrameBuffer();
	auto& fwdPass = *gfxDevice.createRenderPass(fwdDesc);
	fwdPass.setViewport(Vec2u::zero(), windowSize);

	// Create vertex shader
	const string vtxShaderCode = R"(
#version 450
layout(location = 0) in vec3 vertex;

void main ( void )
{
	gl_Position = vec4(vertex.xy, 0.0, 1.0);
}
)";
	Pipeline::ShaderModule::Descriptor vtxDesc;
	vtxDesc.code = { vtxShaderCode };
	vtxDesc.stage = Pipeline::ShaderModule::Descriptor::Vertex;

	auto vtxShader = gfxDevice.createShaderModule(vtxDesc);
	if(vtxShader.id == Pipeline::InvalidId)
		return -1;

	// Create pixel shader
	const string pxlShaderCode = R"(
#version 450
out lowp vec3 outColor;

void main (void) {	
	outColor = vec3(1.0);
}
)";
	Pipeline::ShaderModule::Descriptor pxlDesc;
	pxlDesc.code = { pxlShaderCode };
	pxlDesc.stage = Pipeline::ShaderModule::Descriptor::Pixel;

	auto pxlShader = gfxDevice.createShaderModule(pxlDesc);
	if(vtxShader.id == Pipeline::InvalidId)
		return -1;

	// Create the pipeline
	Pipeline::Descriptor pipelineDesc;
	pipelineDesc.vtxShader = vtxShader;
	pipelineDesc.pxlShader = pxlShader;
	auto pipeline = gfxDevice.createPipeline(pipelineDesc);

	// Create a quad
	auto quad = rev::graphics::RenderGeom::quad({0.5f, 0.5f});

	// Command buffer to draw a simple quad
	CommandBuffer cmdBuffer;
	cmdBuffer.setPipeline(pipeline);
	cmdBuffer.setVertexData(quad.getVao());// Bind vtx data
	cmdBuffer.drawTriangles(quad.indices().count, CommandBuffer::IndexType::U16);// Draw triangles

	// Record command buffer into the pass
	fwdPass.record(cmdBuffer);
	
	// Main loop
	for(;;)
	{
		if(!rev::core::OSHandler::get()->update())
			break;

		// Send pass to the GPU
		renderQueue.submitPass(fwdPass);

		// Finish frame
		renderQueue.present();
	}

	// Clean up
	gfxDevice.destroyRenderPass(fwdPass);
	FileSystem::end();
	return 0;
}
