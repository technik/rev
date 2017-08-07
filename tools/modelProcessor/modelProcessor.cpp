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
		size_t nOffset = format.normalOffset();
		size_t uvOffset = format.uvOffset();
		for (size_t i = 0; i < nVertices; ++i) {
			auto dataRow = &((uint8_t*)_dst)[stride*i];
			if(format.hasPosition) {
				auto v0 = (Vec3f*)dataRow;
				*v0 = vertices[i];
			}
			if(format.normalFmt != VertexFormat::UnitVecFormat::eNone) {
				auto n0 = (Vec3f*)(&dataRow[nOffset]);
				*n0 = normals[i];
			}
			if(format.nUVs > 0) {
				auto uv0 = (Vec2f*)(&dataRow[uvOffset]);
				*uv0 = uvs[i];
			}
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
		format.serialize(_out);
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
	// Vertex positions
	_dst.format.hasPosition = true;
	_dst.nVertices = _mesh->mNumVertices;
	_dst.vertices = reinterpret_cast<Vec3f*>(_mesh->mVertices);
	// Normals
	if(_mesh->HasNormals()) {
		_dst.format.normalFmt = VertexFormat::UnitVecFormat::e3Vec3f;
		_dst.format.normalSpace = VertexFormat::NormalSpace::eModel;
		_dst.normals = reinterpret_cast<Vec3f*>(_mesh->mNormals);
	}
	// UVs
	size_t nUVs = _mesh->GetNumUVChannels();
	if(nUVs) {
		if (nUVs > 1) {
			cout << "Error: Too many uv channels. Only 1 is supported\n";
			return false;
		}
		_dst.format.nUVs = 1;
		_dst.uvs = new Vec2f[_dst.nVertices];
		auto uv0 = _mesh->mTextureCoords[0];
		for(auto i = 0; i < _dst.nVertices; ++i) {
			_dst.uvs[i] = Vec2f(uv0[i].x, uv0[i].y);
		}
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
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool loadFBX(const string& _src, IntermediateModel& _dst) {
	const aiScene* fbx = fbxLoader.ReadFile(_src, aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices);
	if(!fbx)
		return false;
	const aiNode* sceneRoot = fbx->mRootNode;
	while(sceneRoot && sceneRoot->mNumMeshes == 0)
		sceneRoot = sceneRoot->mChildren[0];
	if(!sceneRoot)
	{
		cout << "Warning: node has no meshes. Nothing will be exported\n";
		return true;
	}
	return loadFBXMesh(fbx->mMeshes[sceneRoot->mMeshes[0]], _dst);
}

//----------------------------------------------------------------------------------------------------------------------
VertexFormat defaultVtxFmt() {
	VertexFormat fmt;
	fmt.hasPosition = true;
	fmt.normalFmt = VertexFormat::UnitVecFormat::e3Vec3f;
	fmt.normalSpace = VertexFormat::NormalSpace::eModel;
	fmt.nUVs = 1;
	return fmt;
}

//----------------------------------------------------------------------------------------------------------------------
bool convertModel(const IntermediateModel& _src, IntermediateModel& _dst, const VertexFormat& _targetFmt) {
	_dst.format = _targetFmt;
	// Copy position
	if(_targetFmt.hasPosition) {
		if(!_src.format.hasPosition)
			return false;
		_dst.nVertices = _src.nVertices;
		_dst.vertices = _src.vertices;
	}
	// Copy normals
	if(_targetFmt.normalFmt != VertexFormat::UnitVecFormat::eNone) {
		if(_targetFmt.normalFmt == _src.format.normalFmt
			&& _targetFmt.normalSpace == _src.format.normalSpace
			&& _targetFmt.normalStorage == _src.format.normalStorage)
		{
			_dst.normals = _src.normals;
		}
		else
			return false;
	}
	// Copy UVs
	if(_targetFmt.nUVs > 0) {
		if(_src.format.nUVs < _targetFmt.nUVs)
			return false;
		_dst.uvs = _src.uvs;
	}
	// Indices
	_dst.nIndices = _src.nIndices;
	_dst.indices = _src.indices;
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	assert(_argc > 1);
	VertexFormat targetFmt = defaultVtxFmt();
	if (_argc <= 1)
	{
		cout << "Missing arguments\n";
		return -1;
	}
	for(int i = 1; i < _argc; ++i) {
		string arg(_argv[i]);
		if(arg.substr(0, 5) == "-nouv")
			targetFmt.nUVs = 0;
	}
	string modelName = _argv[1];
	string src = modelName + ".fbx";
	string dst = modelName + ".rmd";
	cout << src << " >> " << dst << "\n";
	// Load
	IntermediateModel uncompressedModel;
	if (!loadFBX(src, uncompressedModel))
	{
		cout << "Error loading model: " << src << "\n";
		return -1;
	}
	// Convert
	IntermediateModel targetModel;
	if(!convertModel(uncompressedModel, targetModel, targetFmt)) {
		cout << "Error: Unable to convert model to target format\n";
		return -1;
	}
	// Save
	ofstream outFile(dst, std::ofstream::binary);
	if (outFile.is_open()) {
		targetModel.saveToStream(outFile);
		outFile.close();
	}
	else {
		cout << "Error: Unable to open output file\n";
		return -2;
	}
	return 0;
}