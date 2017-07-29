//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags

#include <math/algebra/vector.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>

#include <video/graphics/geometry/VertexFormat.h>

using namespace std;
using namespace rev::video;
using namespace rev::math;

typedef rev::video::VertexFormat	VertexFormat;

// --- Global data ---
// The importer is stored globally, so buffers don't get erased and we can save some memcpy's
Assimp::Importer fbxLoader;

struct IntermediateModel {
	VertexFormat format;
	Vec3f* vertices = nullptr;
	Vec3f* normals = nullptr;
	Vec2f* uvs = nullptr;
	uint16_t* indices = nullptr;
	uint32_t nIndices = 0;
	uint16_t nVertices = 0;

	void collapseVertexData(void* _dst, size_t stride) {
		size_t nOffset = sizeof(Vec3f);
		size_t uvOffset = nOffset + sizeof(Vec3f);
		for (size_t i = 0; i < nVertices; ++i) {
			auto dataRow = &((uint8_t*)_dst)[stride*i];
			auto v0 = (Vec3f*)dataRow;
			auto n0 = (Vec3f*)(&dataRow[nOffset]);
			auto uv0 = (Vec2f*)(&dataRow[uvOffset]);
			*v0 = vertices[i];
			*n0 = normals[i];
			*uv0 = uvs[i];
		}
	}
	
	bool saveToStream(ostream& _out) {
		auto stride = format.stride();
		// Allocate space for cache-friendly (interleaved) vertex data
		float* vertexData = reinterpret_cast<float*>(new uint8_t[stride*nVertices]);
		if(!vertexData) {
			cout << "Error: Unable to allocate memory\n";
			return false;
		}
		collapseVertexData(vertexData, stride);
		// Save header
		_out.write((char*)&nVertices, sizeof(nVertices));
		_out.write((char*)&nIndices, sizeof(nIndices));
		// Save vertex data
		_out.write((const char*)vertexData, stride*(size_t)nVertices);
		if(!_out)
			return false;
		// Save index data
		_out.write((const char*)indices, sizeof(uint16_t)*(size_t)nIndices);
		if (!_out)
			return false;
		return true;
	}
};

//----------------------------------------------------------------------------------------------------------------------
bool loadFBXMesh(const aiMesh* _mesh, IntermediateModel& _dst) {
	// Vertex data
	_dst.nVertices = _mesh->mNumVertices;
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
	_dst.vertices = reinterpret_cast<Vec3f*>(_mesh->mVertices);
	_dst.normals = reinterpret_cast<Vec3f*>(_mesh->mNormals);
	_dst.uvs = new Vec2f[_dst.nVertices];
	auto uv0 = _mesh->mTextureCoords[0];
	for(auto i = 0; i < _dst.nVertices; ++i) {
		_dst.uvs[i] = Vec2f(uv0[i].x, uv0[i].y);
	}
	// Index data
	_dst.nIndices = _mesh->mNumFaces * 3;
	_dst.indices = new uint16_t[_dst.nIndices];
	for (size_t i = 0; i < _mesh->mNumFaces; ++i) {
		_dst.indices[3 * i + 0] = _mesh->mFaces[i].mIndices[0];
		_dst.indices[3 * i + 1] = _mesh->mFaces[i].mIndices[1];
		_dst.indices[3 * i + 2] = _mesh->mFaces[i].mIndices[2];
	}
	// Format
	_dst.format.
	_dst.format.normals = VertexFormat::NormalFormat::normal3;
	_dst.format.uvChannels = 1;
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool loadFBX(const string& _src, IntermediateModel& _dst) {
	const aiScene* fbx = fbxLoader.ReadFile(_src, aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices);
	if(!fbx)
		return false;
	const aiNode* sceneRoot = fbx->mRootNode;
	if(sceneRoot->mNumMeshes == 0)
	{
		cout << "Warning: node has no meshes. Nothing will be exported\n";
		return true;
	}
	return loadFBXMesh(fbx->mMeshes[sceneRoot->mMeshes[0]], _dst);
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
	string dst = modelName + ".rmd";
	cout << src << " >> " << dst << "\n";

	IntermediateModel uncompressedModel;
	if (!loadFBX(src, uncompressedModel))
	{
		cout << "Error loading model: " << src << "\n";
		return -1;
	}
	ofstream outFile(dst, std::ofstream::binary);
	if (outFile.is_open()) {
		uncompressedModel.saveToStream(outFile);
		outFile.close();
	}
	else {
		cout << "Error: Unable to open output file\n";
		return -2;
	}
	return 0;
}