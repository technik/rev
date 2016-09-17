//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/18
//----------------------------------------------------------------------------------------------------------------------
// File system interface


#include "fileSystem.h"
#include "file.h"
#include <cassert>
#ifdef ANDROID
#include <android/asset_manager.h>
#endif // ANDROID

namespace rev {
	namespace core {
#ifdef ANDROID
		void FileSystem::init(AAssetManager* _mgr) {
			File::setAssetMgr(_mgr);
		}

		void FileSystem::end() {
			// 
		}
#endif // ANDROID

#ifdef _WIN32
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
#endif // _WIN32

	} // namespace core
}	// namespace rev
