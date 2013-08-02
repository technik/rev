//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/29
//----------------------------------------------------------------------------------------------------------------------
// Windows File system

#ifdef _WIN32

#include "fileSystemWindows.h"
#include <Windows.h>

#include <fstream>
using namespace std;

namespace rev { namespace platform {

	//------------------------------------------------------------------------------------------------------------------
	FileSystemWindows* 	FileSystemWindows::sFileSystem = nullptr;

	//------------------------------------------------------------------------------------------------------------------
	FileSystem* FileSystem::get()
	{
		return FileSystemWindows::get();
	}
	
	//------------------------------------------------------------------------------------------------------------------
	FileSystemWindows* FileSystemWindows::get()
	{
		if(nullptr == sFileSystem)
			sFileSystem = new FileSystemWindows;
		return sFileSystem;
	}

	//------------------------------------------------------------------------------------------------------------------
	FileSystemWindows::FileBuffer FileSystemWindows::getFileAsBuffer(const char* _fileName)
	{
		FileBuffer returnBuffer = { 0, nullptr };

		fstream fileStream;
		// Open the file
		fileStream.open(_fileName, ios_base::binary|ios_base::in);
		if(fileStream.is_open())
		{
			// Measure it's size
			fileStream.seekg(0, ios::end);
			returnBuffer.mSize = int(fileStream.tellg());
			fileStream.seekg(0, ios::beg);
			returnBuffer.mSize -= int(fileStream.tellg());;
			// Allocate the buffer
			returnBuffer.mBuffer = new char[returnBuffer.mSize+1];
			// Fill the buffer with the contents of the file
			fileStream.read((char*)returnBuffer.mBuffer, returnBuffer.mSize);
			((char*)returnBuffer.mBuffer)[returnBuffer.mSize] = '\0';
			fileStream.close();
		}

		return returnBuffer;
	}
	
	//------------------------------------------------------------------------------------------------------------------
	void FileSystemWindows::update()
	{
		// Check
		LPOVERLAPPED over;
		if(GetQueuedCompletionStatus(mPortHandle, &mBytesRecieved, &mCompletionKey, &over, 0))
		{
			if(mBytesRecieved > 0) {
				// Copy file name
				FILE_NOTIFY_INFORMATION * notifyInfo = reinterpret_cast<FILE_NOTIFY_INFORMATION*>(mNotificationBuffer);
				std::string filename;
				DWORD i = 0;
				DWORD length = notifyInfo->FileNameLength / sizeof(wchar_t);
				for(; i < length; ++i) {
					char c = (char)notifyInfo->FileName[i];
					if(c == '\0')
						break;
					filename += c;
				}
				doFileChanged(filename);
			}

			watchDirectory();
		}
	}
	
	//------------------------------------------------------------------------------------------------------------------
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
				FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED ,
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
	void FileSystemWindows::doFileChanged(const std::string& _fileName)
	{
		auto res = mFileChangedEvents.find(_fileName);
		if(res != mFileChangedEvents.end())
			res->second(_fileName.c_str());
	}

	//------------------------------------------------------------------------------------------------------------------
	void FileSystemWindows::watchDirectory()
	{
		if(!ReadDirectoryChangesW(mDirectoryHandle, mNotificationBuffer,
			mNotifBufferSize * sizeof(uint32_t),
			true,					// Watch subtree
			FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_CREATION | FILE_NOTIFY_CHANGE_ATTRIBUTES | FILE_NOTIFY_CHANGE_SIZE,
			&mBytesRecieved, &mOverlapped, NULL))
		{
			DWORD error = GetLastError();
			printf("Error %d: Fail monitoring directory", error);
		}
	}

}	// namespace platform
}	// namespace rev

#endif // _WIN32