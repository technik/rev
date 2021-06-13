//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once

#include <gfx/backend/Vulkan/Vulkan.h>
#include <gfx/Texture.h>
#include <math/algebra/vector.h>

namespace rev::gfx
{
	class EnvironmentProbe
	{
	public:
		~EnvironmentProbe() = default;

		std::shared_ptr<Texture> texture() const { return m_cubemapTexture; }
		uint32_t numMipmaps() const { return m_numMipmaps; }

		// Cos-weighted incoming radiance baked into spherical harmonics.
		// w: Band 0, xyz: Band 1.
		const math::Vec4f& SHIrradiance() const { return m_shIrradiance;  }

		// Load probe from an HDR file
		static std::shared_ptr<EnvironmentProbe> LoadProbe(std::string_view imageName, uint32_t numLevels, uint32_t maxResolution);
	private:
		EnvironmentProbe(
			std::shared_ptr<Texture> texture,
			uint32_t numMipmaps,
			math::Vec4f shIrradiance)
			: m_cubemapTexture(texture)
			, m_numMipmaps(numMipmaps)
			, m_shIrradiance(shIrradiance)
		{}

		std::shared_ptr<Texture> m_cubemapTexture;
		uint32_t m_numMipmaps;
		math::Vec4f m_shIrradiance;
	};
}
