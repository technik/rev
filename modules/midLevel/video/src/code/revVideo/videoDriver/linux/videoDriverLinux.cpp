//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On October 24th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Linux video driver

#include "videoDriverLinux.h"
#include <revCore/codeTools/assert/assert.h>

using rev::math::Vec2u;

namespace rev { namespace video 
{
	//------------------------------------------------------------------------------------------------------------------
	VideoDriverLinux::VideoDriverLinux()
	{
		// Intentionally blank
	}

	//------------------------------------------------------------------------------------------------------------------
	Vec2u	VideoDriverLinux::screenResolution () const
	{
		return Vec2u::zero();
	}

	//------------------------------------------------------------------------------------------------------------------
	Driver3d* VideoDriverLinux::createDriver3d()
	{
		// TODO
		revAssert(false, "3d driver creation is not yet supported under linux systems");
		return 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	Window* VideoDriverLinux::createWindow(const math::Vec2i& position, const math::Vec2u& resolution)
	{
		// TODO
		return 0;
	}

}	// namespace video
}	// namespace rev