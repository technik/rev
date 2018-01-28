//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <math/algebra/vector.h>
#include <game/scene/component.h>
#include <game/scene/scene.h>
#include <game/scene/transform/transform.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

using namespace std;
using namespace rev::math;

// --- Global data ---
// The importer is stored globally, so buffers don't get erased and we can save some memcpy's
Assimp::Importer fbxLoader;

struct IntermediateModel {
	//VertexFormat format;
	Vec3f* vertices = nullptr;
	Vec3f* normals = nullptr;
	Vec2f* uvs = nullptr;
	uint16_t* indices = nullptr;

	uint32_t nIndices = 0;
	uint32_t nVertices = 0;

	struct VertexData {
		Vec3f position, normal;
		Vec2f uv;
	};

	//----------------------------------------------------------------------------------------------------------------------
	void collapseVertexData(vector<VertexData>& _dst) const 
	{
		for (size_t i = 0; i < nVertices; ++i) {
			auto& dataRow = _dst[i];
			dataRow.position = vertices[i];
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

struct MeshRendererDesc : public rev::game::Component {
	int32_t meshIdx = -1;
	int32_t materialIdx = -1;

	void serialize(std::ostream& _out) const override {
		_out << "MeshRenderer";
		_out.write((const char*)meshIdx, sizeof(meshIdx));
		_out.write((const char*)materialIdx, sizeof(materialIdx));
	}
};

struct SceneDesc
{
	std::vector<rev::game::SceneNode> nodes;
	std::vector<IntermediateModel> meshes;

	void saveToStream(std::ostream& out) const
	{
		// Save header
		struct header {
			uint32_t nMeshes, nNodes;
		} header;
		out.write((const char*)&header, sizeof(header));
		// Write meshes
		for(auto& mesh : meshes)
			mesh.saveToStream(out);
		// Write nodes
		for(auto& node : nodes)
			node.serialize(out);
	}
};

//----------------------------------------------------------------------------------------------------------------------
bool loadFBX(const string& _src, SceneDesc& _dst) {
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
		auto node = stack.back().first;
		auto parentIdx = stack.back().second;
		stack.pop_back();
		// Push children for processing
		for(unsigned i = 0; i < node->mNumChildren; ++i)
			stack.push_back({node->mChildren[i], _dst.nodes.size()});
		// Actually process the node
		_dst.nodes.push_back(rev::game::SceneNode());
		auto& dst = _dst.nodes.back();
		dst.name = node->mName.C_Str();
		if(node->mNumMeshes > 0)
		{
			auto mesh = new MeshRendererDesc;
			mesh->meshIdx = node->mMeshes[0];
			dst.addComponent(mesh);
		}
		// Add transform
		auto xForm = new rev::game::Transform();
		memcpy(xForm->matrix().data(), &node->mTransformation, 12*sizeof(float));
		dst.addComponent(xForm);
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
	string dst = modelName + ".scn";
	cout << src << " >> " << dst << "\n";

	SceneDesc scene;
	if (!loadFBX(src, scene))
	{
		cout << "Error loading model: " << src << "\n";
		return -1;
	}
	ofstream outFile(dst, std::ofstream::binary);
	if (outFile.is_open()) {
		scene.saveToStream(outFile);
		outFile.close();
	}
	else {
		cout << "Error: Unable to open output file\n";
		return -2;
	}
	return 0;
}