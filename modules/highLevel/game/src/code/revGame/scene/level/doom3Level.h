//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/Aug/17
//----------------------------------------------------------------------------------------------------------------------
// Doom 3 Level
#ifndef _REV_GAME_SCENE_LEVEL_DOOM3LEVEL_H_
#define _REV_GAME_SCENE_LEVEL_DOOM3LEVEL_H_

#include <map>
#include <string>
#include <vector>

#include <revGraphics3d/renderable/renderable.h>

namespace rev { namespace game {

	class Doom3Level {
	public:
		Doom3Level(const char* _fileName);
		math::Vec3f playerStart;

	private:
		class IDToken {
		public:
			IDToken():mType(Int){}
			IDToken(IDToken&&);
			IDToken(const IDToken&);
			~IDToken();
			IDToken& operator=(IDToken&&);
			IDToken& operator=(const IDToken&);
			enum Type { Float, Int, String, Id } mType;
			union { float f; int i; char* c; } mData;
			float asFloat() { return mType==Float?mData.f:float(mData.i);}
		};

		struct IDModel {
		public:
			std::vector<rev::graphics3d::Renderable*> mSurfaces;
			void setOrigin(const math::Vec3f&);
		};

		int mCursor;
		int mOpenBraces;
		const char* mBuffer;
		std::string mMapName;
		std::map<std::string, IDModel*> mModels;

		void parseMapFile();
		void parseEntity();
		void parseChunk();
		void skipChunk();
		void parseModelChunk();
		IDToken parseToken();
		bool readChar(char);
		void skipSpacesAndComments();
		IDToken parseNumber();
		IDToken parseString();
		IDToken parseId();
		
		// static bool getFileLine(const char* _fileBuffer, char* _lineBuffer, unsigned& _cursor);
		// static void parseModelHeader(const char* _lineBuffer, string& _name, unsigned& _nSurfaces);
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_LEVEL_DOOM3LEVEL_H_