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

	// Command buffer to draw a simple quad
	CommandBuffer cmdBuffer;

	// Renderpass
	RenderPass::Descriptor fwdDesc;
	float grey = 0.5f;
	fwdDesc.clearColor = { grey,grey,grey, 1.f };
	fwdDesc.clearFlags = RenderPass::Descriptor::Clear::Color;
	auto& fwdPass = *gfxDevice.createRenderPass(fwdDesc);
	fwdPass.setViewport(windowStart, windowSize);

	// Create vertex shader
	const string vtxShaderCode = R"(
layout(location = 0) in vec2 vertex;

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
