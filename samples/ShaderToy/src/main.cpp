//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Little shadertoy implementation
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
	auto wnd = createWindow(windowStart, windowSize, "ShaderToy", true);

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

	*OSHandler::get() += [&](MSG _msg) {
		if(_msg.message == WM_SIZING || _msg.message == WM_SIZE)
		{
			// Get new rectangle size without borders
			RECT clientSurface;
			GetClientRect(_msg.hwnd, &clientSurface);
			windowSize = Vec2u(clientSurface.right, clientSurface.bottom);
			fwdPass.setViewport(Vec2u::zero(), windowSize);
			return true;
		}

		//if(rev::input::KeyboardInput::get()->processWin32Message(_msg))
		//	return true;
		return false;
	};

	// Create vertex shader
	const string vtxShaderCode = R"(
#version 430
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
#version 430
out lowp vec3 outColor;

layout(location = 0) uniform vec4 t;
layout(location = 1) uniform vec4 Window;

void main (void) {	
	vec2 uv = gl_FragCoord.xy / Window.xy;
	outColor = vec3(uv.x,uv.y,sin(t.y));
}
)";
	Pipeline::ShaderModule::Descriptor pxlDesc;
	pxlDesc.code = { pxlShaderCode };
	pxlDesc.stage = Pipeline::ShaderModule::Descriptor::Pixel;

	auto pxlShader = gfxDevice.createShaderModule(pxlDesc);
	if(pxlShader.id == Pipeline::InvalidId)
		return -1;

	// Create the pipeline
	Pipeline::Descriptor pipelineDesc;
	pipelineDesc.vtxShader = vtxShader;
	pipelineDesc.pxlShader = pxlShader;
	auto pipeline = gfxDevice.createPipeline(pipelineDesc);

	// Create a quad
	auto quad = rev::gfx::RenderGeom::quad({2.f, 2.f});

	// Command buffer to set pipeline and other common stuff
	CommandBuffer setupCmd;
	setupCmd.setPipeline(pipeline);

	// Command buffer with chaning uniforms
	CommandBuffer uniformCmd;
	CommandBuffer::UniformBucket timeUniform;

	// Command buffer to draw a simple quad
	CommandBuffer quadCmd;
	quadCmd.setVertexData(quad.getVao());// Bind vtx data
	quadCmd.drawTriangles(quad.indices().count, CommandBuffer::IndexType::U16);// Draw triangles

	// Record all command buffers into the pass
	fwdPass.record(setupCmd);
	fwdPass.record(uniformCmd);
	fwdPass.record(quadCmd);
	
	// Main loop
	float t = 0; // t modulo seconds
	float T = 0; // Total T
	unsigned numSeconds = 0;
	for(;;)
	{
		if(!rev::core::OSHandler::get()->update())
			break;

		// Modify the uniform command
		auto tVector = Vec4f(t,T,t*t,sin(Pi*t));
		timeUniform.clear();
		timeUniform.vec4s.push_back({0, tVector});
		timeUniform.vec4s.push_back({1, {float(windowSize.x()), float(windowSize.y()), 0.f, 0.f}});
		uniformCmd.clear();
		uniformCmd.setUniformData(timeUniform);

		// Send pass to the GPU
		renderQueue.submitPass(fwdPass);

		// Finish frame
		renderQueue.present();

		// Update time
		t += 1.f/60;
		if(t > 1)
		{
			t -= 1.f;
			numSeconds++;
		}
		T = t + numSeconds;
	}

	// Clean up
	gfxDevice.destroyRenderPass(fwdPass);
	FileSystem::end();
	return 0;
}
