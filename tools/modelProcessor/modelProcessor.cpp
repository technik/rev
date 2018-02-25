//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <math/algebra/vector.h>
#include <game/scene/component.h>
#include <game/scene/ComponentLoader.h>
#include <game/scene/scene.h>
#include <game/scene/sceneNode.h>
#include <game/scene/transform/transform.h>
#include <graphics/scene/renderGeom.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace rev::math;
using namespace rev::game;

// --- Global data ---
// The importer is stored globally, so buffers don't get erased and we can save some memcpy's
Assimp::Importer fbxLoader;

struct IntermediateModel {
	//VertexFormat format;
	Vec3f* vertices = nullptr;
	Vec3f* tangents = nullptr;
	Vec3f* bitangents = nullptr;
	Vec3f* normals = nullptr;
	Vec2f* uvs = nullptr;
	uint16_t* indices = nullptr;

	uint32_t nIndices = 0;
	uint32_t nVertices = 0;

	using VertexData = rev::graphics::RenderGeom::Vertex;

	//----------------------------------------------------------------------------------------------------------------------
	void collapseVertexData(vector<VertexData>& _dst) const 
	{
		for (size_t i = 0; i < nVertices; ++i) {
			auto& dataRow = _dst[i];
			dataRow.position = vertices[i];
			dataRow.tangent = tangents[i];
			dataRow.bitangent = bitangents[i];
			dataRow.normal = normals[i];
			dataRow.uv = uvs[i];
		}
	}

	//----------------------------------------------------------------------------------------------------------------------
	bool loadFBXMesh(const aiMesh* _mesh) {
		// Vertex data
		nVertices = _mesh->mNumVertices;
		bool hasNormals = _mesh->HasNormals();
		bool hasUVs = _mesh->GetNumUVChannels() > 0;
		if (!hasNormals || !hasUVs) {
			cout << "Error: Mesh must have normals and uvs\n";
			return false;
		}
		if (_mesh->GetNumUVChannels() > 1) {
			cout << "Error: Too many uv channels. Only 1 is supported\n";
			return false;
		}
		vertices = reinterpret_cast<Vec3f*>(_mesh->mVertices);
		tangents = reinterpret_cast<Vec3f*>(_mesh->mTangents);
		bitangents = reinterpret_cast<Vec3f*>(_mesh->mBitangents);
		normals = reinterpret_cast<Vec3f*>(_mesh->mNormals);
		uvs = new Vec2f[nVertices];
		auto uv0 = _mesh->mTextureCoords[0];
		for(uint32_t i = 0; i < nVertices; ++i) {
			uvs[i] = Vec2f(uv0[i].x, uv0[i].y);
		}
		// Index data
		nIndices = _mesh->mNumFaces * 3;
		indices = new uint16_t[nIndices];
		for (size_t i = 0; i < _mesh->mNumFaces; ++i) {
			indices[3 * i + 0] = _mesh->mFaces[i].mIndices[0];
			indices[3 * i + 1] = _mesh->mFaces[i].mIndices[1];
			indices[3 * i + 2] = _mesh->mFaces[i].mIndices[2];
		}
		return true;
	}
	
	//----------------------------------------------------------------------------------------------------------------------
	bool saveToStream(ostream& _out) const
	{
		// Allocate space for cache-friendly vertex data
		vector<VertexData> vertexData(nVertices);
		collapseVertexData(vertexData);
		// Save header
		_out.write((char*)&nVertices, sizeof(nVertices));
		_out.write((char*)&nIndices, sizeof(nIndices));
		// Save vertex data
		_out.write((const char*)vertexData.data(), sizeof(VertexData)*(size_t)nVertices);
		if(!_out)
			return false;
		// Save index data
		_out.write((const char*)indices, sizeof(uint16_t)*(size_t)nIndices);
		if (!_out)
			return false;
		return true;
	}
};

struct MeshRendererDesc : public Component {
	vector<uint32_t> meshIndices;
	string modelScene;
};

struct SceneDesc
{
	std::vector<shared_ptr<SceneNode>> nodes;
	std::vector<int>				parents;
	std::vector<IntermediateModel> meshes;

	void saveMeshes(ostream& dst)
	{
		// --- Serialize meshes ---
		uint32_t nMeshes = meshes.size();
		dst.write((const char*)&nMeshes, sizeof(nMeshes));
		for(auto& mesh : meshes)
			mesh.saveToStream(dst);
	}

	void saveSceneLayout(ostream& dst)
	{
		// --- Serialize node tree ---
		// Build node tree
		Scene tree;
		// Skip root node
		tree.root()->name = nodes[0]->name;
		assert(nodes[0]->components().size() == 1);
		for(size_t i = 1; i < nodes.size(); ++i)
		{
			auto parentNdx = parents[i];
			assert(parentNdx != -1);
			if(parentNdx == 0)
				tree.root()->addChild(nodes[i]);
			else
				nodes[parentNdx]->addChild(nodes[i]);
		}
	}
};

//----------------------------------------------------------------------------------------------------------------------
void processFBXNode(SceneNode& dstNode, const aiNode& fbxNode, const std::string& modelsFile)
{
	dstNode.name = fbxNode.mName.C_Str();
	if(fbxNode.mNumMeshes > 0)
	{
		auto mesh = make_unique<MeshRendererDesc>();
		mesh->modelScene = modelsFile;
		mesh->meshIndices.resize(fbxNode.mNumMeshes);
		for(unsigned i = 0; i < fbxNode.mNumMeshes; ++i)
		{
			mesh->meshIndices[i] = fbxNode.mMeshes[i];
		}
		dstNode.addComponent(move(mesh));
	}
	// Add transform
	auto tPose = fbxNode.mTransformation;
	Mat44f xFormMatrix;
	memcpy(xFormMatrix.data(), &tPose.Transpose(), 16*sizeof(float));
	auto xForm = make_unique<rev::game::Transform>();
	xForm->matrix() = xFormMatrix.block<3,4>(0,0);
	dstNode.addComponent(move(xForm));
}

//----------------------------------------------------------------------------------------------------------------------
bool loadFBX(const string& _src, const string& modelsFile, SceneDesc& _dst) {
	const aiScene* fbx = fbxLoader.ReadFile(
		_src,
		aiProcess_JoinIdenticalVertices | aiProcess_CalcTangentSpace
	);
	if(!fbx)
		return false;
	// Load all meshes
	_dst.meshes.resize(fbx->mNumMeshes);
	for(size_t i = 0; i < _dst.meshes.size(); ++i)
	{
		auto& fbxMesh = fbx->mMeshes[i];
		_dst.meshes[i].loadFBXMesh(fbxMesh);
	}
	// Load scene nodes
	std::vector<std::pair<const aiNode*, int>> stack; // pair<node, parent index>
	stack.push_back({ fbx->mRootNode, -1 });
	while(!stack.empty())
	{
		// Extract node from stack
		auto fbxNode = stack.back().first;
		_dst.parents.push_back(stack.back().second);
		stack.pop_back();
		auto parentIdx = _dst.nodes.size();
		// Push children for processing
		for(unsigned i = 0; i < fbxNode->mNumChildren; ++i)
			stack.push_back({fbxNode->mChildren[i], parentIdx});
		// Actually process the node
		auto dstNode = make_shared<SceneNode>();
		_dst.nodes.push_back(dstNode);
		processFBXNode(*dstNode, *fbxNode, modelsFile);
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	assert(_argc > 1);
	if (_argc <= 1)
	{
		cout << "Missing arguments\n";
		return -1;
	}
	string modelName = _argv[1];
	string src = modelName + ".fbx";
	string dstModels = modelName + ".mdl";
	string dstScene = modelName + ".scn";
	cout << src << " >> " << dstModels << " + " << dstScene << "\n";

	SceneDesc scene;
	if (!loadFBX(src, dstModels, scene))
	{
		cout << "Error loading model: " << src << "\n";
		return -1;
	}
	ofstream outModelFile(dstModels, std::ofstream::binary);
	if (outModelFile.is_open()) {
		scene.saveMeshes(outModelFile);
		outModelFile.close();
	}
	else {
		cout << "Error: Unable to write to output file" << dstModels << "\n";
		return -2;
	}
	ofstream outSceneFile(dstScene, std::ofstream::binary);
	if (outSceneFile.is_open()) {
		scene.saveSceneLayout(outSceneFile);
		outSceneFile.close();
	}
	else {
		cout << "Error: Unable to write to output file" << dstScene << "\n";
		return -3;
	}
	return 0;
}