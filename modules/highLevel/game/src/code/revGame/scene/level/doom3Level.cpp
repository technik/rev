//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/Aug/17
//----------------------------------------------------------------------------------------------------------------------
// Doom 3 Level
#include "doom3Level.h"

#include <revCore/codeTools/assert/assert.h>
#include <revPlatform/fileSystem/fileSystem.h>
#include <revVideo/types/image/image.h>

using namespace rev::graphics3d;
using namespace rev::math;
using namespace rev::platform;
using namespace rev::video;

namespace rev { namespace game {

	//------------------------------------------------------------------------------------------------------------------
	Doom3Level::IDToken::~IDToken()
	{
		if((mType==String||mType==Id)&&nullptr!= mData.c)
			delete[]mData.c;
	}

	//------------------------------------------------------------------------------------------------------------------
	Doom3Level::IDToken::IDToken(IDToken&& _t)
		:mType(_t.mType)
		,mData(_t.mData)
	{
		_t.mData.c = nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	Doom3Level::IDToken::IDToken(const IDToken& _t)
		:mType(_t.mType)
	{
		if(mType==IDToken::Id || mType==IDToken::String) {
			unsigned textLen = strlen(_t.mData.c);
			mData.c = new char[textLen];
			strcpy(mData.c, _t.mData.c);
		} else mData = _t.mData;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::IDModel::setOrigin(const math::Vec3f& _origin)
	{
		for(auto s : mSurfaces) {
			s->m[0][3] = _origin.x;
			s->m[1][3] = _origin.y;
			s->m[2][3] = _origin.z;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	Doom3Level::IDToken& Doom3Level::IDToken::operator=(IDToken&& _t)
	{
		mType = _t.mType;
		mData = _t.mData;
		_t.mData.c = nullptr;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	Doom3Level::IDToken& Doom3Level::IDToken::operator=(const IDToken& _t)
	{
		mType = _t.mType;
		if(mType==IDToken::Id || mType==IDToken::String) {
			unsigned textLen = strlen(_t.mData.c);
			mData.c = new char[textLen];
			strcpy(mData.c, _t.mData.c);
		} else mData = _t.mData;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	Doom3Level::Doom3Level(const char* _fileName)
		:mCursor(0)
		,mOpenBraces(0)
		,mBuffer(nullptr)
		,mFilterBsp(false)
	{
		std::string baseName = _fileName;
		mBuffer = (const char*)(FileSystem::get()->getFileAsBuffer((baseName+ ".proc").c_str()).mBuffer);
		if(mBuffer != nullptr) {
			mMapName = std::string(parseId().mData.c);	// Read map name (TODO: This is not really the name of the map. it is a magic constant)
			while('\0' != mBuffer[mCursor])
				parseChunk();
		}
		delete[] mBuffer;
		mCursor = 0;
		mBuffer = (const char*)(FileSystem::get()->getFileAsBuffer((baseName+ ".map").c_str()).mBuffer);
		if(mBuffer != nullptr) {
			parseMapFile();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	int Doom3Level::pointInArea(const math::Vec3f& _point) const
	{
		if(mAreaTree.mNodes.empty())
			return -1;
		const BspTree::Node* node = &mAreaTree.mNodes[0];
		int			nodeNum;

		while( 1 ) {
			float d = _point * node->normal + node->offset;
			if (d > 0) {
				nodeNum = node->positiveChildIdx;
			} else {
				nodeNum = node->negativeChildIdx;
			}
			if ( nodeNum == 0 ) {
				return -1;		// in solid
			}
			if ( nodeNum < 0 ) {
				nodeNum = -1 - nodeNum;
				if ( nodeNum >= mAreas.size() ) {
					revAssert(false, "idRenderWorld::PointInArea: area out of range");
				}
				return nodeNum;
			}
			node = &mAreaTree.mNodes[nodeNum];
		}

		return -1;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::traverseBspTree(const math::Vec3f& _camPos, std::function<void (const Renderable*)> _fn, std::function<bool (const Renderable*)> _filter) const
	{
		auto applyFn = [=](const Renderable* _r) { if(_filter(_r)) _fn(_r); };
		mAreasRendered.clear();
		mAreasRendered.resize(mAreas.size(), false);

		int areaId = pointInArea(_camPos);
		if(-1 !=  areaId) {
			IDModel * area = mAreas[areaId];
			for(auto s : area->mSurfaces)
				applyFn(s);
		}
		// traverseNode(_camPos, 0, applyFn);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::traverseNode(const math::Vec3f& _camPos, unsigned _nodeIdx, std::function<void (const Renderable*)> _fn) const
	{
		const BspTree::Node& node = mAreaTree.mNodes[_nodeIdx];

		//float camProj = _camPos * node.normal;
		bool posSide = _camPos * node.normal - node.offset < 0.f;
		int child = posSide?node.positiveChildIdx:node.negativeChildIdx;

		if(child > 0)
			traverseNode(_camPos, child, _fn);
		else if (child < 0) {
			unsigned areaId = unsigned(-1 -child);
			if(!mAreasRendered[areaId]) {
				for(auto surface : mAreas[areaId]->mSurfaces)
					_fn(surface);
				mAreasRendered[areaId] = true;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::traverse(const math::Vec3f& _camPos, std::function<void (const Renderable*)> _fn, std::function<bool (const Renderable*)> _filter) const
	{
		auto apply = [=](const IDModel* _area) {
			for(auto surface : _area->mSurfaces)
				if(_filter(surface))
					_fn(surface);
		};

		if(mFilterBsp)
			traverseBspTree(_camPos, _fn, _filter);
		else
			for(auto area : mAreas)
				apply(area);
		// for(auto model : mModels)
		// 	apply(model.second);
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::parseMapFile()
	{
		parseId(); parseNumber(); // Version
		while('\0' != mBuffer[mCursor])
		{
			parseEntity();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::parseEntity()
	{
		if(readChar('{'))
		{
			bool isStaticMesh = false;
			bool isPlayerStart = false;
			std::string modelName;
			Vec3f origin;

			skipSpacesAndComments();
			char c = mBuffer[mCursor];
			while(c == '\"') // Descriptor string
			{
				IDToken descriptor = parseString();
				if(!strcmp(descriptor.mData.c, "classname")) {
					IDToken className = parseString();
					if(!strcmp(className.mData.c, "func_static"))
						isStaticMesh = true;
					else if(!strcmp(className.mData.c, "info_player_start"))
						isPlayerStart = true;
				} else if(!strcmp(descriptor.mData.c, "name")) {
					modelName = std::string(parseString().mData.c);
				} else if(!strcmp(descriptor.mData.c, "origin")) {
					readChar('\"');
					origin.x = parseNumber().asFloat();
					origin.y = parseNumber().asFloat();
					origin.z = parseNumber().asFloat();
					readChar('\"');
				} else {
					parseString(); // Discard descriptor
				}
				skipSpacesAndComments();
				c = mBuffer[mCursor];
			}
			// Done with descriptors, skip primitives and brushes
			while(!readChar('}')) {
				skipChunk();
			}
			if(isStaticMesh) {
				auto modelIt = mModels.find(modelName);
				if(modelIt != mModels.end())
					modelIt->second->setOrigin(origin);
			}
			else if(isPlayerStart) {
				playerStart = origin;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::parseChunk()
	{
		IDToken nextToken = parseToken();
		revAssert(nextToken.mType==IDToken::Id);
		if(!strcmp(nextToken.mData.c, "model")) {
			parseModelChunk();
		} else if(!strcmp(nextToken.mData.c, "nodes")) {
			parseBspTreeChunk();
		}else
			skipChunk();
		// Hide models
		// for(auto model : mModels)
		// 	for(auto surface : model.second->mSurfaces)
		// 		surface->isVisible = false;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::parseBspTreeChunk()
	{
		if(readChar('{'))
		{
			int nNodes = parseNumber().mData.i;
			mAreaTree.reserve(nNodes);
			while(!readChar('}'))
			{
				BspTree::Node node;
				// Read node
				readChar('(');
				node.normal.x = parseNumber().asFloat();
				node.normal.y = parseNumber().asFloat();
				node.normal.z = parseNumber().asFloat();
				node.offset = parseNumber().asFloat();
				readChar(')');
				node.positiveChildIdx = parseNumber().mData.i;
				node.negativeChildIdx = parseNumber().mData.i;

				mAreaTree.mNodes.push_back(node);
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::skipChunk()
	{
		if(readChar('{')) {
			unsigned depth = 1;
			while(depth) {
				char c = mBuffer[mCursor++];
				if		(c=='{') ++depth;
				else if	(c=='}') --depth;
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::parseModelChunk()
	{
		if(readChar('{'))
		{
			IDModel* model = new IDModel;
			std::string modelName = parseString().mData.c;
			bool isAreaModel = !strncmp("_area", modelName.c_str(), 5);
			int nSurfaces = parseNumber().mData.i;
			for(int i = 0; i < nSurfaces; ++i)
			{
				if(readChar('{')) {
					std::string textureName = std::string(parseString().mData.c) + ".tga"; // Surface texture
					unsigned nVerts = parseNumber().mData.i;
					unsigned nIndices = parseNumber().mData.i;
					Vec3f* vertices = new Vec3f[nVerts];
					Vec3f* normals = new Vec3f[nVerts];
					Vec2f* uvs = new Vec2f[nVerts];
					for(unsigned v = 0; v < nVerts; ++v) { // Read vertices
						readChar('(');
						vertices[v].x = parseNumber().asFloat();
						vertices[v].y = parseNumber().asFloat();
						vertices[v].z = parseNumber().asFloat();
						uvs[v].x = parseNumber().asFloat();
						uvs[v].y = parseNumber().asFloat();
						normals[v].x = parseNumber().asFloat();
						normals[v].y = parseNumber().asFloat();
						normals[v].z = parseNumber().asFloat();
						readChar(')');
					}
					uint16_t* indices = new uint16_t[nIndices];
					for(unsigned f = 0; f < nIndices; ++f) {
						indices[f] = parseNumber().mData.i;
					}
					readChar('}');
					Renderable* surface = new Renderable;
					surface->m = Mat34f::identity();
					surface->setVertexData(nVerts, vertices, normals, uvs);
					surface->setFaceIndices(nIndices, indices);
					// Get texture
					const Image* texture = nullptr;
					auto texIter = mTextures.find(textureName);
					if(texIter == mTextures.end()) {
						texture = Image::loadImage(textureName.c_str());
						mTextures.insert(std::make_pair(textureName.c_str(), texture));
					}
					else texture = texIter->second;
					surface->setTexture(texture);
					model->mSurfaces.push_back(surface);
				} else assert(false);
			}
			if(isAreaModel)
				mAreas.push_back(model);
			else
				mModels[modelName] = model;
			readChar('}');
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	Doom3Level::IDToken Doom3Level::parseToken()
	{
		char c = mBuffer[mCursor];
		if(c == '-' || (c >='0' && c<='9'))
			return parseNumber();
		else if(c=='\"')
			return parseString();
		else return parseId();
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Doom3Level::readChar(char _c)
	{
		skipSpacesAndComments();
		if(mBuffer[mCursor]==_c) {
			++mCursor;
			return true;
		} else return false;
	}

	//------------------------------------------------------------------------------------------------------------------
	Doom3Level::IDToken Doom3Level::parseNumber()
	{
		skipSpacesAndComments();
		IDToken token;
		token.mType = IDToken::Int;
		unsigned cursor = mCursor; // Keep a local copy to be able to reconstruct the number after first parsing pass
		if(mBuffer[mCursor]=='-')
			++mCursor;
		char c = mBuffer[mCursor];
		while(c>='0'&&c<='9') // Skip all digits, we're only interested in knowing wether the number is a float
			c = mBuffer[++mCursor];
		if(c == '.') // Is float
		{
			token.mType = IDToken::Float;
			c = mBuffer[++mCursor];
			while(c>='0'&&c<='9') // Skip all digits till the end of the number
				c = mBuffer[++mCursor];
			if(c=='e') {
				c = mBuffer[++mCursor];
				if(c=='-') // Exponent's sign
					c = mBuffer[++mCursor];
				while(c>='0'&&c<='9') // Skip all digits till the end of the number
					c = mBuffer[++mCursor];
			}
			token.mData.f = (float)atof(&mBuffer[cursor]);
		} else // Is integer
		{
			token.mData.i = atoi(&mBuffer[cursor]);
		}
		return token;
	}

	//------------------------------------------------------------------------------------------------------------------
	Doom3Level::IDToken Doom3Level::parseString()
	{
		skipSpacesAndComments();
		IDToken token;
		token.mType = IDToken::String;
		++mCursor; // Skip first "
		unsigned cursor = 0;
		token.mData.c = new char[256];
		char c = mBuffer[mCursor++];
		while(c != '\"')
		{
			token.mData.c[cursor++] = c;
			c = mBuffer[mCursor++];
		}
		token.mData.c[cursor] = '\0';
		++mCursor; // Skip final "
		return token;
	}

	//------------------------------------------------------------------------------------------------------------------
	Doom3Level::IDToken Doom3Level::parseId()
	{
		skipSpacesAndComments();
		IDToken token;
		token.mType = IDToken::Id;
		token.mData.c = new char[64];
		char c = mBuffer[mCursor++];
		unsigned cursor = 0;
		while((c>='a'&&c<='z')||(c>='A'&&c<='Z')||(c>='0'&&c<='9')||c=='_') {
			token.mData.c[cursor++] = c;
			c = mBuffer[mCursor++];
		}
		token.mData.c[cursor] = '\0';
		return token;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Doom3Level::skipSpacesAndComments()
	{
		enum ParseState { Normal, LineComment, BlockComment} parseState = Normal;
		char c = mBuffer[mCursor];
		while(c != '\0') { switch(parseState)
		{
		case Normal:
			if(c == '/') {
				char c1 = mBuffer[mCursor+1];
				if(c1 == '/') {
					parseState = LineComment;
					break;
				} else if(c1 == '*') {
					parseState = BlockComment;
					++mCursor;
					break;
				}
			}else if(c==' ' || c=='\t' || c=='\n' || c=='\r') {
				c = mBuffer[++mCursor];
			} else
				return;
			break;
		case LineComment: // Skip till the end of the line
			while(c != '\n')
				c = mBuffer[++mCursor];
			parseState = Normal;
			break;
		case BlockComment:
			while(parseState == BlockComment)
			{
				if(c == '*') {
					c = mBuffer[++mCursor];
					if(c == '/')
						parseState = Normal;
				}
				c = mBuffer[++mCursor];
			}
			break;
		}}
	}

}	// namespace game
}	// namespace rev