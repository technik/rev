//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
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

#include "../sceneNode.h"
#include <memory>
#include <string>
#include <vector>

namespace fx::gltf { struct Document; }
namespace rev::game { class SceneNode; }

namespace rev::gfx
{
	class RenderContextVulkan;
	class VulkanAllocator;
	class GPUBuffer;
}

namespace rev::game {

	class GltfLoader
	{
	public:
		GltfLoader(gfx::RenderContextVulkan&);
		~GltfLoader();

		struct LoadResult
		{
			std::shared_ptr<SceneNode> rootNode;
			std::shared_ptr<gfx::GPUBuffer> m_gpuData;
			size_t asyncLoadToken;
		};

		/// Load a gltf scene
		/// filePath must contain folder, file name and extension
		/// \return root node of the loaded asset
		LoadResult load(const std::string& filePath);

	private:
		std::shared_ptr<SceneNode> loadNodes(const fx::gltf::Document&);

		gfx::RenderContextVulkan& m_renderContext;
		gfx::VulkanAllocator& m_alloc;

		std::string m_assetsFolder;
	};

}
