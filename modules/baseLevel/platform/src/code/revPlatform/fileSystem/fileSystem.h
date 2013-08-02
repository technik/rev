//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// File system interface

#ifndef _REV_PLATFORM_FILESYSTEM_FILESYSTEM_H_
#define _REV_PLATFORM_FILESYSTEM_FILESYSTEM_H_

#include <functional>
#include <map>
#include <string>

#include <revCore/event/event.h>

namespace rev { namespace platform {

	class FileSystem
	{
	public:
		struct FileBuffer {
			int		mSize;
			void*	mBuffer;
		};

		typedef rev::Event<void, const char*>	FileEvent;
	public:
		static FileSystem* get(); // Must be implemented in derived class' cpp.

		FileEvent&			onFileChanged	(const std::string& _fileName) { return mFileChangedEvents[_fileName]; }
		virtual FileBuffer	getFileAsBuffer	(const char* _fileName) = 0;

	protected:

		std::map<std::string, FileEvent>	mFileChangedEvents;
	};

}	// namespace platform
}	// namespace rev

#endif // _REV_PLATFORM_FILESYSTEM_FILESYSTEM_H_