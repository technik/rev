//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include <assimp/Importer.hpp>      // C++ importer interface
#include <assimp/scene.h>           // Output data structure
#include <assimp/postprocess.h>     // Post processing flags
#include <experimental/filesystem>

#include <math/algebra/vector.h>
#include <game/scene/sceneNode.h>
#include <video/graphics/renderObj.h>

#include <cassert>
#include <fstream>
#include <iostream>
#include <string>
#include <sstream>

#include <video/graphics/geometry/VertexFormat.h>

using namespace std;
using namespace rev::game;
using namespace rev::video;
using namespace rev::math;

// --- Global data ---
// The importer is stored globally, so buffers don't get erased and we can save some memcpy's
Assimp::Importer fbxLoader;

struct IntermediateMesh {
	VertexFormat format;
	Vec3f* vertices = nullptr;
	Vec3f* normals = nullptr;
	Vec2f* uvs = nullptr;
	uint16_t* indices = nullptr;
	uint32_t nIndices = 0;
	uint16_t nVertices = 0;
	string name;
	
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
private:
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
};

//----------------------------------------------------------------------------------------------------------------------
bool loadFBXMesh(const aiMesh* _mesh, IntermediateMesh& _dst) {
	// meta data
	_dst.name = _mesh->mName.C_Str();
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
VertexFormat defaultVtxFmt() {
	VertexFormat fmt;
	fmt.hasPosition = true;
	fmt.normalFmt = VertexFormat::UnitVecFormat::e3Vec3f;
	fmt.normalSpace = VertexFormat::NormalSpace::eModel;
	fmt.nUVs = 1;
	return fmt;
}

//----------------------------------------------------------------------------------------------------------------------
bool convertMesh(const IntermediateMesh& _src, IntermediateMesh& _dst, const VertexFormat& _targetFmt) {
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
class MeshComponent : public Component {
public:
	MeshComponent(SceneNode& _n) : Component(_n) {}
	void update() override {}

	string meshName;
};

//----------------------------------------------------------------------------------------------------------------------
void loadFBXMeshes(const aiScene& fbx, vector<IntermediateMesh*>& _dst, const VertexFormat& _tgtFmt) {
	_dst.reserve(fbx.mNumMeshes);
	for(size_t i = 0; i < fbx.mNumMeshes; ++i) {
		// Read mesh
		IntermediateMesh uncompressed;
		loadFBXMesh(fbx.mMeshes[i], uncompressed);
		// Convert format
		IntermediateMesh* finalMesh = new IntermediateMesh;
		convertMesh(uncompressed, *finalMesh, _tgtFmt);
		// Ensure the mesh has a name to reference it
		if(finalMesh->name.empty()) { // We always need a name
			stringstream ss;
			ss << i;
			finalMesh->name = ss.str();
		}
		// Add mesh to the list
		_dst.push_back(finalMesh);
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool loadFBX(const string& _src, SceneNode*& _dst, vector<IntermediateMesh*>& _dstMeshes, const VertexFormat& _tgtFmt) {
	const aiScene* fbx = fbxLoader.ReadFile(_src, aiProcess_JoinIdenticalVertices | aiProcess_PreTransformVertices);
	if(!fbx)
		return false;
	loadFBXMeshes(*fbx, _dstMeshes, _tgtFmt);
	vector<aiNode*>	fbxNodeStack;
	vector<pair<SceneNode*,size_t>>	ancestorStack;
	fbxNodeStack.push_back(fbx->mRootNode);
	// Traverse fbx tree
	while(fbxNodeStack.size()) {
		aiNode* fbxNode = fbxNodeStack.back();
		fbxNodeStack.pop_back();
		SceneNode* obj = new SceneNode(2); // Reserve space for transform + mesh
		// Handle hierarchy
		if(ancestorStack.empty())
			_dst = obj;
		else {
			obj->attachTo(ancestorStack.back().first);
			ancestorStack.back().second--;
			if(!ancestorStack.back().second)
				ancestorStack.pop_back();
		}
		if(fbxNode->mNumChildren > 0) {
			ancestorStack.push_back(make_pair(obj,fbxNode->mNumChildren));
		}
		for(size_t i = 0; i < fbxNode->mNumChildren; ++i) {
			fbxNodeStack.push_back(fbxNode->mChildren[i]);
		}
		// Handle transform
		ObjTransform* transform = new ObjTransform(*obj);
		for(size_t i = 0; i < 3; ++i) {
			for(size_t j = 0; j < 4; ++j) {
				transform->matrix()[i][j] = fbxNode->mTransformation[i][j];
			}
		}
		obj->addComponent(transform);
		// Handle mesh references
		for(size_t i = 0; i < fbxNode->mNumMeshes; ++i) {
			IntermediateMesh uncompressed;
			if(!loadFBXMesh(fbx->mMeshes[fbxNode->mMeshes[i]], uncompressed))
				return false;
			MeshComponent* meshComp = new MeshComponent(*obj);
			meshComp->meshName = _dstMeshes[fbxNode->mMeshes[i]]->name;
			obj->addComponent(meshComp);
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------------------------------
void serializeTransform(rev::math::Mat34f& _tr, cjson::Json& _dst) {
	vector<float> numbers(12);
	memcpy(numbers.data(), &_tr, 12*sizeof(float));
	_dst["mat"] = numbers;
	_dst["_type"] = "Transform";
}

//----------------------------------------------------------------------------------------------------------------------
void serializeMeshComponent(const std::string& _meshFolder, MeshComponent* _mesh, cjson::Json& _dst) {
	_dst["file"] = _meshFolder + _mesh->meshName + ".rmd";
	_dst["_type"] = "RenderObj";
}

//----------------------------------------------------------------------------------------------------------------------
void serializeNode(const std::string& _meshFolder, SceneNode* _node, cjson::Json& _dst) {
	_dst["_type"] = "Node";
	// Serialize components
	std::vector<cjson::Json>	components;
	for(auto comp : _node->components()) {
		if(dynamic_cast<ObjTransform*>(comp)) {
			cjson::Json tr;
			serializeTransform(dynamic_cast<ObjTransform*>(comp)->matrix(), tr);
			components.push_back(tr);
		} else
		{
			cjson::Json tr;
			serializeMeshComponent(_meshFolder, dynamic_cast<MeshComponent*>(comp), tr);
			components.push_back(tr);
		}
	}
	if(!components.empty())
		_dst["components"] = components;
	// Serialize children nodes
	std::vector<cjson::Json>	children(_node->children().size());
	if(!children.empty()) {
		size_t i = 0;
		for(auto child : _node->children())
		{
			serializeNode(_meshFolder, child, children[i]);
			++i;
		}
		_dst["children"] = children;
	}
}

//----------------------------------------------------------------------------------------------------------------------
void saveMesh(const std::string& folder, IntermediateMesh& mesh) {
	string dst = "./" + folder + "/" + mesh.name + ".rmd";
	ofstream outFile(dst, std::ofstream::binary);
	if(!outFile.is_open()) {
		cout << "Error: Unable to open dst file " << dst << "\n";
		return;
	}
	mesh.saveToStream(outFile);
}

//----------------------------------------------------------------------------------------------------------------------
void saveMeshes(const std::string& _folder, const vector<IntermediateMesh*>& fileMeshes) {
	// Serialize each mesh
	std::experimental::filesystem::path meshFolderPath = _folder;
	if(!std::experimental::filesystem::exists(meshFolderPath))
		std::experimental::filesystem::create_directory(meshFolderPath);
	for(auto mesh : fileMeshes) {
		saveMesh(_folder, *mesh);
	}
}

//----------------------------------------------------------------------------------------------------------------------
bool saveNodeHierarchy(const std::string& _meshFolder, const std::string& _fileName, SceneNode* _sceneRoot) {
	ofstream outFile(_fileName, std::ofstream::binary);
	if (outFile.is_open()) {
		cjson::Json sceneData;
		serializeNode(_meshFolder, _sceneRoot, sceneData["objects"](0));
		sceneData.serialize(outFile);
		//	targetModel.saveToStream(outFile);
		outFile.close();
		return true;
	}
	else {
		cout << "Error: Unable to open output file\n";
		return false;
	}
}

//----------------------------------------------------------------------------------------------------------------------
int main(int _argc, const char** _argv) {
	assert(_argc > 1);
	// Parse command line
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
	string sceneName = modelName + ".scn";
	cout << src << " >> " << sceneName << "\n";
	// Load FBX
	SceneNode* rootObj = nullptr;
	vector<IntermediateMesh*>	fileMeshes;
	if (!loadFBX(src, rootObj, fileMeshes, targetFmt))
	{
		cout << "Error loading model: " << src << "\n";
		return -1;
	}
	// Save meshes
	saveMeshes(modelName, fileMeshes);
	// Save scene hierarchy
	if(!saveNodeHierarchy(sceneName + "/", sceneName, rootObj))
		return -2;
	return 0;
}