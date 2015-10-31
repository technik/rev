//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple scene manager
#ifndef _REV_GAME_SCENE_SCENEMGR_H_
#define _REV_GAME_SCENE_SCENEMGR_H_

#include <core/components/sceneNode.h>
#include <vector>

namespace rev {
	namespace game {
		
		class SceneMgr {
		public:
			/// Open a scene and return it as a vector of scene nodes.
			/// \return true on succuess importing the scene
			static bool importScene(const char* _fileName, std::vector<core::SceneNode*>& _dst);
		};

	}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_SCENEMGR_H_