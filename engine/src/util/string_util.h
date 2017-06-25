//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <string>

namespace rev {
	namespace util {

		std::string getFileExtension(const std::string& _s) {
			auto pos = _s.rfind('.');
			if (pos == std::string::npos)
				return "";
			else
				return _s.substr(pos);
		}

	}
}
