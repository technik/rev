////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video system
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// Video system

// Engine configuration
#define REV_USE_DIRECT_RENDERER

// Engine headers
#include "video.h"

#include "revCore/codeTools/assert/assert.h"
#include "revVideo/color/color.h"
#include "revVideo/videoDriver/videoDriver.h"
#ifdef _linux
#include "revVideo/videoDriver/opengl/linux/videoDriverLinux.h"
#endif // _linux
#ifdef ANDROID
#include "revVideo/videoDriver/android/videoDriverAndroid.h"
#endif // ANDROID
#ifdef WIN32
#include "revVideo/videoDriver/opengl/windows/videoDriverOGLWindows.h"
#endif // WIN32

#include "revVideo/renderer/renderer3d.h"
#ifdef REV_USE_DIRECT_RENDERER
#include "revVideo/renderer/directRenderer/directRenderer.h"
#endif // REV_USE_DIRECT_RENDERER

// Active namespaces
using namespace rev::codeTools;

namespace rev	{	namespace	video
{
	//--------------------------------------------------------------------------
	// static data definitions
	SVideo * SVideo::s_pVideo = 0;

	//--------------------------------------------------------------------------
	// Function implementations

	//--------------------------------------------------------------------------
	void SVideo::init()
	{
		revAssert(0 == s_pVideo);
		s_pVideo = new SVideo();
		s_pVideo->createRenderer();
	}

	//--------------------------------------------------------------------------
	void SVideo::end()
	{
		revAssert(0 != s_pVideo);
		// TODO: Fix memory leaks!
		delete s_pVideo;
		s_pVideo = 0;
	}

	//--------------------------------------------------------------------------
	void SVideo::update()
	{
		// assert we have a driver
		revAssert(0 != m_pDriver);
		// Begin frame
		m_pDriver->beginFrame();
		// Render the frame
		m_pRenderer->renderFrame();
		// End frame
		m_pDriver->endFrame();
	}

	//--------------------------------------------------------------------------
	SVideo::SVideo():
		m_pDriver(0),
		m_pRenderer(0)
	{
		// Create the video driver and renderer
		createDriver();
	}

	//--------------------------------------------------------------------------
	SVideo::~SVideo()
	{
		// Delete the video driver
		delete m_pDriver;
	}

	//--------------------------------------------------------------------------
	void SVideo::createDriver()
	{
#ifdef _linux
		m_pDriver = new CVideoDriverLinux();
		reinterpret_cast<IVideoDriverOpenGL*>(m_pDriver)->initOpenGL();
#endif // _linux
#ifdef ANDROID
		m_pDriver = new CVideoDriverAndroid();
#endif // ANDROID
#ifdef WIN32
		m_pDriver = new CVideoDriverOGLWindows();
		reinterpret_cast<IVideoDriverOpenGL*>(m_pDriver)->initOpenGL();
#endif // WIN32
		// Assert some platform created a driver
		revAssert(0 != m_pDriver);
	}

	//--------------------------------------------------------------------------
	void SVideo::createRenderer()
	{
#ifdef REV_USE_DIRECT_RENDERER
		m_pRenderer = new CDirectRenderer();
#endif // REV_USE_DIRECT_RENDERER
		// Assert renderer exists
		revAssert(0 != m_pRenderer);
	}

}	// namespace video
}	// namespace rev
