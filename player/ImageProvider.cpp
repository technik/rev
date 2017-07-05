//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#include "ImageProvider.h"
#include <iostream>

namespace rev {
	bool ImageProvider::init(const std::string& _fileName) {
		mSrc.open(_fileName);
		if (!mSrc.isOpened()) {
			std::cout << "Error: Unable to open video \"" << _fileName << "\"\n";
			return false;
		}
		return true;
	}
}