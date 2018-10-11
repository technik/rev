//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// Minecraft-style sample game
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <graphics/scene/renderGeom.h>

namespace vkft::gfx {

	class World
	{
	public:
		World()
		{
			quad = rev::gfx::RenderGeom::quad({0.5f, 0.5f});
		}

		rev::gfx::RenderGeom quad;
	};

}
