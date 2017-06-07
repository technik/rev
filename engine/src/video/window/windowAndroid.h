//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <math/algebra/vector.h>

namespace rev {
	namespace video {

		class WindowAndroid {
		public:
			WindowAndroid(const math::Vec2u& _pos, const math::Vec2u& _size, const char* _windowName);
		};

		typedef WindowAndroid	WindowBase;

} }	// namespace rev::video