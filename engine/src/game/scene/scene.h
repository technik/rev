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
		class TransformSrc; 
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

		private:
			void setSceneRoot(TransformSrc* src);
			TransformSrc* mSceneRoot;

			template<class TreeNode_>
			void traverseTree(TreeNode_* _root, std::function<void(TreeNode_*)> _visitor);
		};

		//--------------------------------------------------------------------------------------------------------------
		template<class TreeNode_>
		void Scene::traverseTree(TreeNode_* _root, std::function<void(TreeNode_*)> _visitor) {
			if(!_root)
				return;
			std::vector<TreeNode*>	nodeStack;
			nodeStack.reserve(1+_root->nChildren());
			nodeStack.push_back(_root);
			while (nodeStack.size()) {
				TreeNode_* parent = nodeStack.back();
				_visitor(parent); // Process parent
				nodeStack.pop_back();
				// Prepare children for processing
				size_t nChildren = parent->nChildren();
				nodeStack.reserve(nodeStack.size() + nChildren);
				for (size_t i = 0; i < nChildren; ++i) {
					nodeStack.push_back(parent->child(i));
				}
			}
		}

	}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_SCENE_H_