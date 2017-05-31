//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <core/world/worldLayer.h>
#include <vector>
#include "logicComponent.h"

namespace rev {
	namespace game {

		class LogicLayer : public core::WorldLayer {
		public:
			bool update(float _dt) override {
				// Init new components
				for (auto c : mUninitialized)
					c->init();
				mUninitialized.clear();
				// Update all objects
				for(auto c : mComponents)
					c->update(_dt);
				mAccumDt = 0.f;
				return true;
			}

			void add(LogicComponent* _c) {
				mUninitialized.push_back(_c);
				mComponents.push_back(_c);
			}

		private:
			float mAccumDt = 0.f;
			std::vector<LogicComponent*>	mUninitialized;
			std::vector<LogicComponent*>	mComponents;
		};
} }
