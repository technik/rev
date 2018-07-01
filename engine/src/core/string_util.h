//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <string>

namespace rev {
	namespace core {

		//--------------------------------------------------------------------------------------------------
		inline std::string getFileExtension(const std::string& _s) {
			auto pos = _s.rfind('.');
			if (pos == std::string::npos)
				return "";
			else
				return _s.substr(pos+1);
		}

		//--------------------------------------------------------------------------------------------------
		inline std::string getPathFolder(const std::string& path)
		{
			auto pos = path.find_last_of("\\/");
			if (pos != std::string::npos)
				return path.substr(0, pos+1);

			return "";
		}

	}
}
