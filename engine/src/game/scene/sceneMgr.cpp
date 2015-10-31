//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple scene manager
#include "sceneMgr.h"
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

namespace rev {
	namespace game {
		//--------------------------------------------------------------------------------------------------------------
		bool SceneMgr::importScene(const char* _fileName, std::vector<core::SceneNode*>& _dst) {
			// Try to open the file
			Assimp::Importer colladaImp;
			const aiScene* colStruct = colladaImp.ReadFile(_fileName,
				aiProcess_GenNormals | aiProcess_Triangulate |
				aiProcess_JoinIdenticalVertices);
			if (!colStruct) {
				return false; // No file exists
			}
			return false;
		}
	}
}