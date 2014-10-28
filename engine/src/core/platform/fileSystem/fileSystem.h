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
#ifdef __linux__
#include "linux/fileSystemLinux.h"
#endif // __linux__

namespace rev {
	namespace core {
		class FileSystem : public FileSystemBase {
		public:
			static FileSystem*	get(); // Must be implemented in derived class' cpp.
			static void init();
			static void end();

			FileEvent&			onFileChanged(const string& _fileName) { return mFileChangedEvents[_fileName]; }
		private:
			FileSystem() = default;
			~FileSystem() = default;
			static FileSystem*	sInstance;
		};
	}
}	// namespace rev

#endif // _REV_CORE_PLATFORM_FILESYSTEM_FILESYSTEM_H_