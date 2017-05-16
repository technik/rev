//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple scene manager
#ifndef _REV_GAME_SCENE_SCENE_H_
#define _REV_GAME_SCENE_SCENE_H_

#include <iostream>
#include <vector>

namespace rev {

	namespace core { class SceneNode; }
	namespace video { class ForwardRenderer; }

	namespace game {
		
		class Scene {
		public:
			Scene();

			bool load(const std::string& _fileName);

			bool update(float _dt);
			void render(ForwardRenderer&);
		private:
			std::vector<core::SceneNode*>	mSceneGraph;
		};

	}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_SCENE_H_