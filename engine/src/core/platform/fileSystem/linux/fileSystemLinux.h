//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014-10-28
//----------------------------------------------------------------------------------------------------------------------
// File system for linux systems
#ifndef _REV_CORE_PLATFORM_FILESYSTEM_LINUX_FILESYSTEMLINUX_H_
#define _REV_CORE_PLATFORM_FILESYSTEM_LINUX_FILESYSTEMLINUX_H_

#include <cstdint>

#include <core/containers/map.h>
#include <core/event.h>
#include <core/types.h>

namespace rev {
	namespace core {
		class FileSystemLinux{
		public:
			typedef Event<const char*> FileEvent;

			void		update();

		protected:
			map<string, FileEvent>	mFileChangedEvents;

		protected:
			FileSystemLinux();
		};

		typedef FileSystemLinux	FileSystemBase;
	}
}	// namespace rev

#endif // _REV_CORE_PLATFORM_FILESYSTEM_LINUX_FILESYSTEMLINUX_H_