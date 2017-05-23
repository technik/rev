//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Simple scene manager
#ifndef _REV_GAME_SCENE_SCENE_H_
#define _REV_GAME_SCENE_SCENE_H_

#include <functional>
#include <iostream>
#include <map>
#include <vector>

namespace cjson { class Json; }

namespace rev {

	namespace core {
		class SceneNode; 
		class Component;
	}
	namespace video {
		class ForwardRenderer;
	}

	namespace game {
		
		class Scene {
		public:
			Scene();
			bool load(const std::string& _fileName);

			bool update(float _dt);
			void render(video::ForwardRenderer&) const;

			// --- Component factories ---
			typedef std::function<core::Component*	(const cjson::Json&)>	Factory;

			void registerFactory(const std::string& type, Factory);

		private:
			core::Component* createComponent(const cjson::Json& _data);
			// Node 0 is always scene's root
			std::vector<core::SceneNode*>			mSceneGraph;
			std::map<std::string,Factory>			mFactories;
		};

	}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_SCENE_H_