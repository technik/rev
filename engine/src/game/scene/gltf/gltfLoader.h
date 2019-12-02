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

#include <string>
#include "../sceneNode.h"
#include <graphics/backend/texture2d.h>
#include <graphics/scene/renderScene.h>
#include <graphics/scene/renderGeom.h>
#include <memory>
#include <vector>

namespace fx::gltf { struct Document; }

namespace rev::game { class SceneNode; }

namespace rev::gfx {
	class Animation;
	class Device;
	class Effect;
	class Material;
	class RenderMesh;
	class SkinInstance;
}

namespace rev::game {

	class GltfLoader
	{
	public:
		GltfLoader(gfx::Device& gfx)
			:m_gfxDevice(gfx)
		{}

		~GltfLoader();

		/// Load a gltf scene
		/// Add renderable content to _gfxWorld
		/// filePath must not contain folder, file name and extension
		/// If parentNode is not nullptr, all the scene nodes will be added as children to it
		void load(
			SceneNode& parentNode,
			const std::string& filePath,
			gfx::RenderScene& _gfxWorld,
			std::vector<std::shared_ptr<SceneNode>>& animNodes,
			std::vector<std::shared_ptr<gfx::Animation>>& _animations);

	private:
		// Shared resources
		std::shared_ptr<gfx::Effect> metallicRoughnessEffect();
		std::shared_ptr<gfx::Effect> specularEffect();
		std::shared_ptr<gfx::Material> defaultMaterial();

		// Load resources
		std::vector<std::shared_ptr<gfx::Material>> loadMaterials(
			const std::string& _assetsFolder,
			const fx::gltf::Document& _document,
			std::vector<gfx::Texture2d>& _textures
		);

		std::vector<std::shared_ptr<gfx::RenderMesh>> loadMeshes(
			const std::vector<gfx::RenderGeom::Attribute>& attributes,
			const fx::gltf::Document& _document,
			const std::vector<std::shared_ptr<gfx::Material>>& _materials);

		std::vector<std::shared_ptr<game::SceneNode>> loadNodes(
			const fx::gltf::Document& _document,
			const std::vector<std::shared_ptr<gfx::RenderMesh>>& _meshes,
			const std::vector<std::shared_ptr<gfx::SkinInstance>>& _skins,
			const std::vector<std::shared_ptr<gfx::Material>>& _materials,
			gfx::RenderScene& _gfxWorld);

		void loadImages(const fx::gltf::Document&);

	private:
		gfx::Device& m_gfxDevice;

		// Cached resources
		gfx::Texture2d m_invalidTexture;
		std::vector<std::shared_ptr<gfx::Image>> m_loadedImages;

		// Supported effects
		std::shared_ptr<gfx::Effect> m_metallicRoughnessEffect;
		std::shared_ptr<gfx::Effect> m_specularEffect;
		std::shared_ptr<gfx::Material> m_defaultMaterial;
	};

}
