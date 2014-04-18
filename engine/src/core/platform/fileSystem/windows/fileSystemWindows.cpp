//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/18
//----------------------------------------------------------------------------------------------------------------------
// File system interface for windows
#ifdef _WIN32

#include "fileSystemWindows.h"
#include <iostream>

namespace rev {
	namespace core {

		//------------------------------------------------------------------------------------------------------------------
		void FileSystemWindows::update()
		{
			// Check
			LPOVERLAPPED over;
			if (GetQueuedCompletionStatus(mPortHandle, &mBytesRecieved, &mCompletionKey, &over, 0))
			{
				if (mBytesRecieved > 0) {
					// Copy file name
					FILE_NOTIFY_INFORMATION * notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(mNotificationBuffer);
					string filename;
					DWORD i = 0;
					DWORD length = notifyInfo->FileNameLength / sizeof(wchar_t);
					for (; i < length; ++i) {
						char c = (char)notifyInfo->FileName[i];
						if (c == '\0')
							break;
						filename += c;
					}
					doFileChanged(filename);
				}

				watchDirectory();
			}
		}

		//--------------------------------------------------------------------------------------------------------------
		FileSystemWindows::FileSystemWindows()
		{
			mOverlapped.Internal = 0;
			mOverlapped.InternalHigh = 0;
			mOverlapped.hEvent = 0;
			mOverlapped.Pointer = 0;
			mDirectoryHandle = CreateFileA(".\\",
				FILE_LIST_DIRECTORY,	// Request permission to list directory contents
				FILE_SHARE_DELETE | FILE_SHARE_READ | FILE_SHARE_WRITE,
				NULL,
				OPEN_EXISTING,
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
				NULL);

			watchDirectory();

			mPortHandle = CreateIoCompletionPort(
				mDirectoryHandle,
				NULL,
				mCompletionKey,
				0
				);
		}

		//------------------------------------------------------------------------------------------------------------------
		void FileSystemWindows::doFileChanged(const string& _fileName)
		{
			auto res = mFileChangedEvents.find(_fileName);
			if (res != mFileChangedEvents.end())
				res->second(_fileName.c_str());
		}

		//------------------------------------------------------------------------------------------------------------------
		void FileSystemWindows::watchDirectory()
		{
			if (!ReadDirectoryChangesW(mDirectoryHandle, mNotificationBuffer,
				mNotifBufferSize * sizeof(uint32_t),
				true,					// Watch subtree
				FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE,
				&mBytesRecieved, &mOverlapped, NULL))
			{
				DWORD error = GetLastError();
				std::cout << "Error " << error << ": Fail monitoring directory\n";
			}
		}
	}	// namespace core
}	// namespace rev

#endif // _WIN32