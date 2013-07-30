//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Windows File system

#ifndef _REV_GAME_APPLICATION_FILESYSTEM_FILESYSTEM_WINDOWS_H_
#define _REV_GAME_APPLICATION_FILESYSTEM_FILESYSTEM_WINDOWS_H_

#ifdef _WIN32

#include "../fileSystem.h"

#include <cstdint>
#include <string>
#include <Windows.h>

namespace rev { namespace game {

	class FileSystemWindows : FileSystem
	{
	public:
		static FileSystemWindows* get();

		void update();
	private:
		static FileSystemWindows* sFileSystem;

		FileSystemWindows();
		void doFileChanged(const std::string& _fileName);
		void watchDirectory();

		static const int	mNotifBufferSize = 1024;
		HANDLE			mDirectoryHandle;
		HANDLE			mPortHandle;
		uint32_t		mNotificationBuffer[mNotifBufferSize];
		DWORD			mBytesRecieved;
		OVERLAPPED		mOverlapped;
		ULONG_PTR		mCompletionKey;
	};

}	// namespace game
}	// namespace rev

#endif // _WIN32

#endif // _REV_GAME_APPLICATION_FILESYSTEM_FILESYSTEM_WINDOWS_H_