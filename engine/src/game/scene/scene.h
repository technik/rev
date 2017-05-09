//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple scene manager
#ifndef _REV_GAME_SCENE_SCENE_H_
#define _REV_GAME_SCENE_SCENE_H_

#include <vector>

namespace rev {

	namespace core { class SceneNode; }

	namespace game {
		
		class Scene {
		public:
			Scene();
			/// Open a scene and return it as a vector of scene nodes.
			/// \return true on succuess importing the scene
			static Scene* import(const char* _fileName);

			void addNode(core::SceneNode* node);
		private:
			std::vector<core::SceneNode*>	mSceneGraph;
		};

	}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_SCENE_H_