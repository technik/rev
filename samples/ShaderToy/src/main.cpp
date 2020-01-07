//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Little shadertoy implementation
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/device.h>
#include <graphics/backend/gpuTypes.h>
#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>
#include <graphics/renderer/renderPass/fullScreenPass.h>
#include <graphics/renderGraph/renderGraph.h>
#include <graphics/renderGraph/frameBufferCache.h>
#include <game/application/base3dApplication.h>
#include <graphics/debug/debugGUI.h>
#include <graphics/debug/imgui.h>

#include <core/platform/cmdLineParser.h>

#include <string>
#include <random>
#include <vector>

using namespace std;
using namespace rev::core;
using namespace rev::gfx;
using namespace rev::math;

class ShaderToy : public rev::game::Base3dApplication
{
public:
	void getCommandLineOptions(CmdLineParser& options) override
	{
		options.addOption("shader", &m_shaderFileName);
	}

	std::string name() override { return "ShaderToy"; }

	bool init() override
	{
		m_timeVector = Vec4f::zero();
		auto& renderuQueue = gfxDevice().renderQueue();
		m_fbCache = std::make_unique<FrameBufferCache>(gfxDevice());
		m_frameGraph = std::make_unique<RenderGraph>(gfxDevice());

		RenderPass::Descriptor fullScreenDesc;
		float grey = 0.5f;
		fullScreenDesc.clearColor = { grey,grey,grey, 1.f };
		fullScreenDesc.clearFlags = Clear::Color;
		fullScreenDesc.target = backBuffer();
		fullScreenDesc.viewportSize = windowSize();
		m_fullScreenPass = gfxDevice().createRenderPass(fullScreenDesc);

		// Actual shader code
		// Create the full screen pass to draw the result
		m_rasterCode = ShaderCodeFragment::loadFromFile(m_shaderFileName);
		m_fullScreenFilter = std::make_unique<FullScreenPass>(gfxDevice(), m_rasterCode);
		m_rasterReloadListener = m_rasterCode->onReload([this](ShaderCodeFragment& fragment)
		{
			m_fullScreenFilter->setPassCode(&fragment);
		});

		// Blue noise
		m_noisePermutations = std::uniform_int_distribution<unsigned>(0, NumBlueNoiseTextures - 1);
		loadNoiseTextures();

		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void loadNoiseTextures()
	{
		// Noise texture sampler
		TextureSampler::Descriptor noiseSamplerDesc;
		noiseSamplerDesc.filter = TextureSampler::MinFilter::Nearest;
		noiseSamplerDesc.wrapS = TextureSampler::Wrap::Repeat;
		noiseSamplerDesc.wrapT = TextureSampler::Wrap::Repeat;

		rev::gfx::Texture2d::Descriptor m_noiseDesc;
		m_noiseDesc.sampler = gfxDevice().createTextureSampler(noiseSamplerDesc);
		m_noiseDesc.depth = false;
		m_noiseDesc.mipLevels = 1;
		m_noiseDesc.pixelFormat.channel = Image::ChannelFormat::Byte;
		m_noiseDesc.pixelFormat.numChannels = 4;
		m_noiseDesc.size = Vec2u(64, 64);
		m_noiseDesc.sRGB = false;

		std::string imageName = "blueNoise/LDR_RGBA_00.png";
		auto digitPos = imageName.find("00");
		for (unsigned i = 0; i < NumBlueNoiseTextures; ++i)
		{
#if 0
			// Create a brand new noise texture
			Vec4f* noise = new Vec4f[64 * 64];
			std::uniform_real_distribution<float> noiseDistrib;
			for (int i = 0; i < 64 * 64; ++i)
			{
				noise[i].x() = noiseDistrib(m_rng);
				noise[i].y() = noiseDistrib(m_rng);
				noise[i].z() = noiseDistrib(m_rng);
				noise[i].w() = noiseDistrib(m_rng);
			}
			m_noiseDesc.srcImages.emplace_back(new rev::gfx::Image(m_noiseDesc.size, noise));
			m_blueNoise[i] = m_device->createTexture2d(m_noiseDesc);
#else
			// Load image from file
			imageName[digitPos] = (i / 10) + '0';
			imageName[digitPos + 1] = (i % 10) + '0';
			auto image = Image::load(imageName, 0);
			m_noiseDesc.srcImages.clear();
			if (image)
			{
				m_noiseDesc.srcImages.push_back(std::move(image));
				m_blueNoise[i] = gfxDevice().createTexture2d(m_noiseDesc);
			}
#endif
		}
	}

	bool updateLogic(float dt) override
	{
		float t = m_timeVector.x();
		float T = m_timeVector.y();
		// Update time
		t += dt;
		T += dt;
		if (t > 1)
		{
			t -= 1.f;
		}
		m_timeVector = Vec4f(t, T, t * t, sin(Pi * t));
		return true;
	}

	void render(float dt) override
	{
		gui::startFrame(windowSize());

		// Build graph
		m_frameGraph->reset();
		m_frameGraph->addPass("Shader pass", windowSize(),
			[this](RenderGraph::IPassBuilder& pass) {
				pass.write(backBuffer());
			},
			[this](const Texture2d*, size_t, CommandBuffer& dst) {
				m_timeUniform.clear();
				m_timeUniform.vec4s.push_back({ 0, m_timeVector });
				m_timeUniform.vec4s.push_back({ 1, {float(windowSize().x()), float(windowSize().y()), 0.f, 0.f} });
				unsigned noiseTextureNdx = m_noisePermutations(m_rng); // New noise permutation for primary light
				m_timeUniform.addParam(2, m_blueNoise[noiseTextureNdx]);
				
				m_fullScreenFilter->render(m_timeUniform, dst);
			});

		m_frameGraph->build(*m_fbCache);

		// Evaluate graph
		m_fsCommandBuffer.clear();
		m_frameGraph->evaluate(m_fsCommandBuffer);
		gfxDevice().renderQueue().submitCommandBuffer(m_fsCommandBuffer);

		// Finish frame
		gui::finishFrame(dt);
		ImGui::Render();
		gfxDevice().renderQueue().present();
	}

private:
	std::string m_shaderFileName = "shaderToy.fx";

	Vec4f m_timeVector;
	RenderPass* m_fullScreenPass;
	std::unique_ptr<RenderGraph> m_frameGraph;
	std::unique_ptr<FrameBufferCache> m_fbCache;
	std::unique_ptr<FullScreenPass> m_fullScreenFilter;

	// Noise
	static constexpr unsigned NumBlueNoiseTextures = 64;
	rev::gfx::Texture2d m_blueNoise[NumBlueNoiseTextures];
	std::default_random_engine m_rng;
	std::uniform_int_distribution<unsigned> m_noisePermutations;

	// Command buffer with changing uniforms
	CommandBuffer::UniformBucket m_timeUniform;
	CommandBuffer m_fsCommandBuffer;

	rev::gfx::ShaderCodeFragment* m_rasterCode = nullptr;
	using ShaderReloadListener = std::shared_ptr<rev::gfx::ShaderCodeFragment::ReloadListener>;
	ShaderReloadListener m_rasterReloadListener;
};

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv)
{	
	ShaderToy application;
	application.run(_argc, _argv);

	return 0;
}
