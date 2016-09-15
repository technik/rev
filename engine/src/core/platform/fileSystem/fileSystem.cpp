//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/18
//----------------------------------------------------------------------------------------------------------------------
// File system interface

#ifdef _WIN32

#include "fileSystem.h"
#include <cassert>

namespace rev {
	namespace core {
		//--------------------------------------------------------------------------------------------------------------
		FileSystem* FileSystem::sInstance = nullptr;

		//--------------------------------------------------------------------------------------------------------------
		FileSystem* FileSystem::get() {
			assert(sInstance);
			return sInstance;
		}

		//--------------------------------------------------------------------------------------------------------------
		void FileSystem::init() {
			assert(!sInstance);
			sInstance = new FileSystem;
		}

		//--------------------------------------------------------------------------------------------------------------
		void FileSystem::end() {
			assert(sInstance);
			delete sInstance;
			sInstance = nullptr;
		}

	}
}	// namespace rev

#endif // _WIN32