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

#include <core/platform/fileSystem/file.h>
#include "ComponentLoader.h"
#include <graphics/scene/renderGeom.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/renderScene.h>
#include "sceneNode.h"

#include <cassert>
#include <iostream>
#include <memory>
#include <string>
#include <vector>

namespace rev { namespace game {

	class Scene
	{
	public:
		using Mesh = graphics::RenderGeom;
		using Node = SceneNode;
		using Json = core::Json;

		std::vector<Node*>& nodes() { return mNodes; }

		Scene(game::RenderScene& _renderScene)
		{
			// Register a factory using scene's loaded meshes
			// TODO: Use scene materials too
			mLoader.registerFactory("MeshRenderer", [&](const std::string&, std::istream& in)
			{
				int32_t meshIdx = 0;
				int32_t materialIdx = 0;
				in.read((char*)&meshIdx, sizeof(meshIdx));
				in.read((char*)&materialIdx, sizeof(materialIdx));
				return _renderScene.createMeshRenderer(mMeshCache[meshIdx]);
			});
		}

		Scene(const Scene&) = delete;
		Scene& operator=(const Scene&) = delete;
		
		void load(std::istream& in)
		{
			// Read header
			struct header {
				uint32_t nMeshes, nNodes;
			} header;
			in.read((char*)&header, sizeof(header));
			assert(mNodes.empty());
			assert(mMeshes.empty());
			mMeshes.resize(header.nMeshes);
			mMeshCache.resize(header.nMeshes);
			mNodes.resize(header.nNodes);
			// Load meshes
			size_t i = 0;
			for(auto& mesh : mMeshes)
			{
				mesh.deserialize(in);
				mMeshCache[i++].reset(&mesh);
			}
			// Load nodes
			// TODO: Reconstruct node hierarchy
			for(auto& node : mNodes)
			{
				node = new SceneNode();
				node->deserialize(mLoader, in);
				node->init();
			}
		}

		void save(const std::string& _fileName)
		{
			/*auto file = std::ofstream(_fileName);
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
				node.serialize(file);*/
		}

	private:
		ComponentLoader						mLoader;
		std::vector<Node*>					mNodes;
		std::vector<std::shared_ptr<graphics::RenderGeom>>	mMeshCache;
		std::vector<graphics::RenderGeom>	mMeshes;
	};

}}
