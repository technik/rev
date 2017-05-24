//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple scene manager
#include "scene.h"

#include <cassert>
#include <cjson/json.h>
#include <video/graphics/renderObj.h>
#include <core/components/sceneNode.h>
#include <video/basicTypes/camera.h>
#include <video/graphics/staticRenderMesh.h>
#include <video/graphics/renderer/forward/forwardRenderer.h>

using namespace rev::core;
using namespace rev::math;
using namespace rev::video;

using namespace std;
using namespace cjson;

namespace rev { namespace game {

	//----------------------------------------------------------------------------------------
	Scene::Scene() {
	}

	//----------------------------------------------------------------------------------------
	bool Scene::load(const string& _fileName) {
		ifstream levelFile(_fileName);
		if(!levelFile.is_open())
			return false;
		Json levelDat;
		if(!levelDat.parse(levelFile))
			return false;
		const Json& sceneGraph = levelDat["graph"];
		// Create scene root and load scene as child to it
		setSceneRoot(new SceneNode(_fileName));
		for (const auto& nodeData : sceneGraph) {
			mSceneRoot->addChild(TransformSrc::construct(nodeData));
		}
		return true;
	}

	//----------------------------------------------------------------------------------------
	bool Scene::update(float _dt) {
		// Traverse tree
		return true;
	}

	//----------------------------------------------------------------------------------------
	void Scene::render(ForwardRenderer& renderer) const {
		// Parse scene graph looking for cameras and renderObj's
		vector<RenderObj*>	activeRObjs;
		vector<Camera*>		activeCams;

		//traverseTree<const SceneNode>(mSceneRoot, [&](const SceneNode* _node) {
		//	for (size_t i = 0; i < _node->nComponents(); ++i)
		//	{
		//		Component* comp = _node->component(i);
		//		if(typeid(*comp) == typeid(Camera))
		//			activeCams.push_back(static_cast<Camera*>(comp));
		//		else if(typeid(*comp) == typeid(RenderObj))
		//			activeRObjs.push_back(static_cast<RenderObj*>(comp));
		//	}
		//});
		for(auto cam : activeCams) {
			assert(cam);
			renderer.render(activeRObjs, *cam);
		}
	}

	//----------------------------------------------------------------------------------------
	void Scene::setSceneRoot(SceneNode* _newRoot) {
		if(mSceneRoot)
			delete mSceneRoot;
		mSceneRoot = _newRoot;
	}

	//----------------------------------------------------------------------------------------
	/*RenderMesh* createMesh(const aiMesh* _mesh) {
		unsigned nVertices = _mesh->mNumVertices;
		Vec3f* vertices = new Vec3f[nVertices];
		Vec3f* normals = new Vec3f[nVertices];
		for (unsigned i = 0; i < nVertices; ++i) {
			vertices[i] = Vec3f(_mesh->mVertices[i].x, _mesh->mVertices[i].y, _mesh->mVertices[i].z);
			normals[i] = Vec3f(_mesh->mNormals[i].x, _mesh->mNormals[i].y, _mesh->mNormals[i].z);
		}
		unsigned nIndices = 3 * _mesh->mNumFaces;
		unsigned short* indices = new unsigned short[nIndices];
		for (unsigned i = 0; i < _mesh->mNumFaces; ++i) {
			indices[3 * i + 0] = _mesh->mFaces[i].mIndices[0];
			indices[3 * i + 1] = _mesh->mFaces[i].mIndices[1];
			indices[3 * i + 2] = _mesh->mFaces[i].mIndices[2];
		}
		RenderMesh* mesh = new RenderMesh;
		mesh->setVertexData(nVertices, vertices, normals, nullptr);
		mesh->setFaceIndices(nIndices, indices, false);
		return mesh;
	}

	//----------------------------------------------------------------------------------------
	Mat34f ai2Mat(aiMatrix4x4 _m) {
		Mat34f m;
		for (unsigned i = 0; i < 3; ++i) {
			m[i][0] = _m[i][0];
			m[i][1] = _m[i][1];
			m[i][2] = _m[i][2];
			m[i][3] = _m[i][3];
		}
		return m;
	}

	//----------------------------------------------------------------------------------------
	void normalizeTr(Mat34f& _m) {
		float factor = 1.f / Vec3f(_m[0][0], _m[0][1], _m[0][2]).norm();
		for (unsigned i = 0; i < 3; ++i) {
			for (unsigned j = 0; j < 4; ++j)
				_m[i][j] *= factor;
		}
	}

	//----------------------------------------------------------------------------------------
	Mat34f globalTransform(const aiNode* _node) {
		// Get absolute transform
		Mat34f transform = ai2Mat(_node->mTransformation);
		for (auto p = _node->mParent; p->mParent; p = p->mParent)
			transform = ai2Mat(p->mTransformation) * transform;
		normalizeTr(transform);
		return transform;
	}

		
		//--------------------------------------------------------------------------------------------------------------
		Scene* Scene::import(const char* _fileName) {
			// Try to open the file
			Assimp::Importer colladaImp;
			const aiScene* colScene = colladaImp.ReadFile(_fileName,
				aiProcess_GenNormals | aiProcess_Triangulate |
				aiProcess_JoinIdenticalVertices);
			if (!colScene) {
				return nullptr; // No file exists
			}

			Scene* scene = new Scene;
			const aiNode* colRoot = colScene->mRootNode;
			unsigned nNodes = colRoot->mNumChildren + 1; // Root plus children
			std::vector<const aiNode*> colNodes;
			colNodes.push_back(colRoot);
			for (unsigned i = 0; i < colRoot->mNumChildren; ++i)
				colNodes.push_back(colRoot->mChildren[i]);
			for (auto colNode : colNodes) {
				if (colNode->mNumMeshes == 0)
					continue;
				RenderMesh* mesh = createMesh(colScene->mMeshes[colNode->mMeshes[0]]);
				RenderObj* obj = new RenderObj(mesh);
				scene->mRenderContext->insert(obj);
				SceneNode* node = new SceneNode();
				obj->attachTo(node);
				std::string name = colNode->mName.C_Str();
				node->setTransform(globalTransform(colNode));
			}
			return scene;
		}*/
	}
}