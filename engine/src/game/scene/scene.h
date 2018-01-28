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

#include <cassert>
#include "sceneNode.h"
#include "ComponentLoader.h"
#include <string>
#include <vector>
#include <graphics/scene/renderGeom.h>
#include <game/scene/meshRenderer.h>
#include "sceneNode.h"
#include <memory>
#include <core/platform/fileSystem/file.h>
#include <fstream>
#include <game/scene/renderScene.h>

namespace rev { namespace game {

	class Scene
	{
	public:
		using Mesh = graphics::RenderGeom;
		using Node = SceneNode;
		using Json = core::Json;

		Scene(const game::RenderScene& _renderScene)
		{
			// Register a factory using scene's loaded meshes
			// TODO: Use scene materials too
			mLoader.registerFactory("MeshRenderer", [&](const std::string&, std::istream& in)
			{
				int32_t meshIdx = 0;
				int32_t materialIdx = 0;
				in.read((char*)meshIdx, sizeof(meshIdx));
				in.read((char*)materialIdx, sizeof(materialIdx));
				return std::make_unique<MeshRenderer>(_renderScene.renderables()[meshIdx]);
			});
		}
		
		void load(const std::string& _fileName)
		{
			auto file = std::ifstream(_fileName);
			// Read header
			struct header {
				uint32_t nMeshes, nNodes;
			} header;
			file.read((char*)&header, sizeof(header));
			assert(mMeshes.empty()); // Scene reusing not supported
			assert(mNodes.empty());
			mMeshes.resize(header.nMeshes);
			mNodes.resize(header.nNodes);
			// Load meshes
			for(auto& mesh : mMeshes)
			{
				mesh = new Mesh();
				mesh->deserialize(file);
			}
			// Load nodes
			for(auto& node : mNodes)
				node.deserialize(mLoader, file);
		}

		void save(const std::string& _fileName)
		{
			auto file = std::ofstream(_fileName);
			// Write scene header
			Json header = {
				{ "nMeshes", mMeshes.size() },
				{ "nNodes", mNodes.size() }
			};
			file << header;
			// Serialize meshes
			for(auto mesh : mMeshes)
				mesh->serialize(file);
			// Serialize nodes
			for(auto& node : mNodes)
				node.serialize(file);
		}

	private:
		ComponentLoader		mLoader;
		std::vector<Node>	mNodes;
	};

}}
