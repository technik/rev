//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/18
//----------------------------------------------------------------------------------------------------------------------
// File system interface for windows
#ifndef _REV_CORE_PLATFORM_FILESYSTEM_WINDOW_FILESYSTEMWINDOWS_H_
#define _REV_CORE_PLATFORM_FILESYSTEM_WINDOW_FILESYSTEMWINDOWS_H_

#include <Windows.h>
#include <cstdint>

#include <core/containers/map.h>
#include <core/event.h>
#include <core/types.h>

namespace rev {
	namespace core {
		class FileSystemWindows {
		public:
			typedef Event<const char*> FileEvent;

			void		update();

		protected:
			map<string, FileEvent>	mFileChangedEvents;

		protected:
			FileSystemWindows();

		private:
			void doFileChanged(const string& _fileName);
			void watchDirectory();

			static const int	mNotifBufferSize = 1024;
			HANDLE				mDirectoryHandle;
			HANDLE				mPortHandle;
			uint32_t			mNotificationBuffer[mNotifBufferSize];
			DWORD				mBytesRecieved;
			OVERLAPPED			mOverlapped;
			ULONG_PTR			mCompletionKey;
		};

		typedef FileSystemWindows	FileSystemBase;
	}
}	// namespace rev

#endif // _REV_CORE_PLATFORM_FILESYSTEM_WINDOW_FILESYSTEMWINDOWS_H_