//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <string>

namespace rev {
	namespace video {

		class Effect {
		public:
			static Effect* loadFromFile(const std::string& _fileName);

		private:
			std::string mShader;
			bool blend = false;
			bool zWrite = false;
		};

	}
}