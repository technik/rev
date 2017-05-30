//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <core/components/component.h>

namespace rev {
	namespace game {

		class LogicComponent : public core::Component {
		public:
			virtual void init() {}
			virtual bool update(float _dt) = 0;
		};
	}
}
