//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
// Little shadertoy implementation
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/device.h>
#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>
#include <graphics/backend/renderPass.h>
#include <graphics/backend/Windows/windowsPlatform.h>
#include <graphics/renderer/renderPass/fullScreenPass.h>
#include <graphics/renderGraph/renderGraph.h>
#include <game/application/base3dApplication.h>

#include <core/platform/cmdLineParser.h>

#include <string>
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

		RenderPass::Descriptor fullScreenDesc;
		float grey = 0.5f;
		fullScreenDesc.clearColor = { grey,grey,grey, 1.f };
		fullScreenDesc.clearFlags = RenderPass::Descriptor::Clear::Color;
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

		return true;
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

	void render(float) override
	{
		m_timeUniform.clear();
		m_timeUniform.vec4s.push_back({ 0, m_timeVector });
		m_timeUniform.vec4s.push_back({ 1, {float(windowSize().x()), float(windowSize().y()), 0.f, 0.f} });

		m_fsCommandBuffer.clear();
		m_fsCommandBuffer.beginPass(*m_fullScreenPass);
		m_fullScreenFilter->render(m_timeUniform, m_fsCommandBuffer);

		// Finish frame
		gfxDevice().renderQueue().submitCommandBuffer(m_fsCommandBuffer);
		gfxDevice().renderQueue().present();
	}

	void onResize() {
		m_fullScreenPass->setViewport({ 0,0 }, windowSize());
	}

private:
	std::string m_shaderFileName = "shaderToy.fx";

	Vec4f m_timeVector;
	RenderPass* m_fullScreenPass;
	std::unique_ptr<FullScreenPass> m_fullScreenFilter;
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
