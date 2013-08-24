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

#include <revGraphics3d/renderable/bspTree.h>
#include <revGraphics3d/renderable/renderable.h>
#include <revGraphics3d/renderable/renderScene.h>
#include <revVideo/types/image/image.h>
#include <functional>

namespace rev { namespace game {

	class Doom3Level : public rev::graphics3d::RenderScene {
	public:
		Doom3Level(const char* _fileName);

		void	traverse	(const math::Vec3f& _camPos
							, std::function<void (const rev::graphics3d::Renderable*)> _fn
							, std::function<bool (const rev::graphics3d::Renderable*)> _filter) const;

		math::Vec3f playerStart;

		unsigned mMaxRenderables;

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
		std::map<std::string, const rev::video::Image*>	mTextures;
		std::vector<IDModel*> mAreas;
		rev::graphics3d::BspTree mAreaTree;

		void processNode(const math::Vec3f& _camPos,
			const rev::graphics3d::BspTree::Node& _node,
			std::vector<bool>& renderedLeafs
			,std::function<void (const IDModel* _area)>) const;

		void parseMapFile();
		void parseEntity();
		void parseChunk();
		void parseBspTreeChunk();
		void skipChunk();
		void parseModelChunk();
		IDToken parseToken();
		bool readChar(char);
		void skipSpacesAndComments();
		IDToken parseNumber();
		IDToken parseString();
		IDToken parseId();
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_LEVEL_DOOM3LEVEL_H_