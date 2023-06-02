//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/18
//----------------------------------------------------------------------------------------------------------------------
// File system interface for windows
#ifndef _REV_CORE_PLATFORM_FILESYSTEM_WINDOW_FILESYSTEMWINDOWS_H_
#define _REV_CORE_PLATFORM_FILESYSTEM_WINDOW_FILESYSTEMWINDOWS_H_

#define VC_EXTRALEAN
#define WIN32_LEAN_AND_MEAN

#include <Windows.h>
#include <cstdint>

#include <string>
#include <core/event.h>
#include <map>

namespace rev {
	namespace core {
		class FileSystemWindows {
		public:
			typedef Event<const char*> FileEvent;

			void		update();
			FileEvent&			onFileChanged(const std::string& _fileName) { return mFileChangedEvents[_fileName]; }

		protected:
			std::map<const std::string, FileEvent>	mFileChangedEvents;

		protected:
			FileSystemWindows();

		private:
			void doFileChanged(const std::string& _fileName);
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