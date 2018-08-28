//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
//----------------------------------------------------------------------------------------------------------------------
#include <core/platform/fileSystem/fileSystem.h>
#include <core/platform/osHandler.h>
#include <graphics/backend/device.h>
#include <graphics/backend/Windows/windowsPlatform.h>
#include <graphics/backend/OpenGL/deviceOpenGLWindows.h>

//--------------------------------------------------------------------------------------------------------------

//--------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	// Init engine core systems
	rev::core::OSHandler::startUp();
	rev::core::FileSystem::init();

	// Create the application window
	auto wnd = rev::gfx::createWindow({100, 150}, { 200, 200 }, "Vulkraft", true);
	// Init graphics
	auto gfxDevice = rev::gfx::DeviceOpenGLWindows(wnd, true);
	auto& renderQueue = gfxDevice.renderQueue();

	// Renderpass
	rev::gfx::RenderPass::Descriptor fwdDesc;
	float grey = 0.5f;
	fwdDesc.clearColor = { grey,grey,grey, 1.f };
	fwdDesc.clearFlags = rev::gfx::RenderPass::Descriptor::Clear::Color;
	auto& fwdPass = *renderQueue.createRenderPass(fwdDesc);
	
	// Main loop
	for(;;)
	{
		if(!rev::core::OSHandler::get()->update())
			break;

		// Send clear pass to the GPU
		renderQueue.submitPass(fwdPass);

		// Finish frame
		renderQueue.present();
	}

	// Clean up
	renderQueue.destroyRenderPass(fwdPass);
	rev::core::FileSystem::end();
	//rev::core::OSHandler::shutDown();
	return 0;
}
