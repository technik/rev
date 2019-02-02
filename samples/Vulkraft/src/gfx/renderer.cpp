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
#include <graphics/backend/texture2d.h>
#include <graphics/scene/camera.h>

#include <string>

using namespace rev::gfx;
using namespace rev::math;

namespace vkft::gfx
{
	//------------------------------------------------------------------------------------------------------------------
	Renderer::Renderer(rev::gfx::DeviceOpenGLWindows& device, const Vec2u& targetSize)
		: mGfxDevice(device)
		, m_rasterPass(device)
	{
		// Create reusable texture descriptor
		TextureSampler::Descriptor bufferSamplerDesc;
		bufferSamplerDesc.filter = TextureSampler::MinFilter::Nearest;
		bufferSamplerDesc.wrapS = TextureSampler::Wrap::Clamp;
		bufferSamplerDesc.wrapT = TextureSampler::Wrap::Clamp;
		m_rtBufferSampler = mGfxDevice.createTextureSampler(bufferSamplerDesc);

		// Create an intermediate buffer of the right size
		onResizeTarget(targetSize);

		// Create the full screen pass to draw the result
		m_rasterCode = new ShaderCodeFragment(R"(
			#ifdef PXL_SHADER

			// Global state
			layout(location = 0) uniform vec4 uWindow;
			layout(location = 1) uniform sampler2D uRayTracedBuffer;

			//------------------------------------------------------------------------------	
			vec3 shade () {
				vec2 uv = gl_FragCoord.xy / uWindow.xy;
				return texture(uRayTracedBuffer, uv).xyz;
			}

			#endif
			)");
		m_rasterPass.setPassCode(m_rasterCode);

		RenderPass::Descriptor fullScreenDesc;
		float grey = 0.5f;
		fullScreenDesc.clearColor = { grey,grey,grey, 1.f };
		fullScreenDesc.clearDepth = 1.0;
		fullScreenDesc.clearFlags = RenderPass::Descriptor::Clear::All;
		fullScreenDesc.target = mGfxDevice.defaultFrameBuffer();
		fullScreenDesc.viewportSize = targetSize;
		m_finalPass = mGfxDevice.createRenderPass(fullScreenDesc);

		// Show compute device limits on console
		auto& limits = mGfxDevice.getDeviceLimits();
		std::cout << "Compute group max count: " << limits.computeWorkGroupCount.x() << " " << limits.computeWorkGroupCount.y() << " " << limits.computeWorkGroupCount.z() << "\n";
		std::cout << "Compute group max size: " << limits.computeWorkGroupSize.x() << " " << limits.computeWorkGroupSize.y() << " " << limits.computeWorkGroupSize.z() << "\n";
		std::cout << "Compute max invokations: " << limits.computeWorkGruopTotalInvokes << "\n";

		// Create compute shader
		m_raytracer = mGfxDevice.createComputeShader({R"(
layout(local_size_x = 1, local_size_y = 1) in;
layout(rgba32f, binding = 0) writeonly uniform image2D img_output;

void main() {
	// base pixel colour for image
	vec4 pixel = vec4(1.0, 0.0, 0.0, 1.0);
	// get index in global work group i.e x,y position
	ivec2 pixel_coords = ivec2(gl_GlobalInvocationID.xy);
  
	//
	// interesting stuff happens here later
	//
  
	// output to a specific pixel in the image
	imageStore(img_output, pixel_coords, pixel);
}
			)"});
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::render(const World& world, const rev::gfx::Camera& camera)
	{
		CommandBuffer commands;
		CommandBuffer::UniformBucket uniforms;
		
		uniforms.addParam(0, m_raytracingTexture);
		commands.dispatchCompute(m_raytracer, uniforms, Vec3i{ (int)m_targetSize.x(), (int)m_targetSize.y(), 1});
		// Prepare pass data
		Vec4f uWindow;
		uniforms.clear();
		uWindow.x() = (float)m_targetSize.x();
		uWindow.y() = (float)m_targetSize.y();
		uniforms.addParam(0, uWindow);
		uniforms.addParam(1, m_raytracingTexture);

		// Render graph
		// Dispathc compute shader
		commands.beginPass(*m_finalPass);
		commands.memoryBarrier(CommandBuffer::MemoryBarrier::ImageAccess);
		m_rasterPass.render(uniforms, commands);

		// Submit
		mGfxDevice.renderQueue().submitCommandBuffer(commands);
		mGfxDevice.renderQueue().present();
	}

	//------------------------------------------------------------------------------------------------------------------
	void Renderer::onResizeTarget(const rev::math::Vec2u& newSize)
	{
		m_targetSize = newSize;
		mTargetFov = float(newSize.x()) / newSize.y();

		// Delete previous target if needed
		if(m_raytracingTexture.isValid())
		{
			mGfxDevice.destroyTexture2d(m_raytracingTexture);
		}

		// Create the raytracing texture
		Texture2d::Descriptor bufferDesc;
		bufferDesc.depth = false;
		bufferDesc.mipLevels = 1;
		bufferDesc.pixelFormat.channel = Image::ChannelFormat::Float32;
		bufferDesc.pixelFormat.numChannels = 4;
		bufferDesc.providedImages = 0;
		bufferDesc.sampler = m_rtBufferSampler;
		bufferDesc.size = newSize;
		bufferDesc.sRGB = false;
		m_raytracingTexture = mGfxDevice.createTexture2d(bufferDesc);
	}
}