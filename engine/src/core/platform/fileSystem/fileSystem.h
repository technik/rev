//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/18
//----------------------------------------------------------------------------------------------------------------------
// File system interface
#ifndef _REV_CORE_PLATFORM_FILESYSTEM_FILESYSTEM_H_
#define _REV_CORE_PLATFORM_FILESYSTEM_FILESYSTEM_H_

#ifdef _WIN32
#include "windows/fileSystemWindows.h"
#endif // _WIN32
#ifdef ANDROID
#include <android/asset_manager.h>
#endif // ANDROID

namespace rev {
	namespace core {
#ifdef _WIN32
		class FileSystem : public FileSystemBase {
#else
		class FileSystem {
#endif
		public:
#ifdef ANDROID
			static void init(AAssetManager* _mgr);
			static void end();
#endif // ANDROID
#ifdef _WIN32
			static void init();
			static void end();
			static FileSystem*	get();
#endif // _WIN32
		private:
			FileSystem() = default;
			~FileSystem() = default;
#ifdef _WIN32
			static FileSystem*	sInstance;
#endif // _WIN32
		};
	}
}	// namespace rev

#endif // _REV_CORE_PLATFORM_FILESYSTEM_FILESYSTEM_H_