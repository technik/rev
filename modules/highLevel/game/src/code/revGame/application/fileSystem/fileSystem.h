//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// File system interface

#ifndef _REV_GAME_APPLICATION_FILESYSTEM_FILESYSTEM_H_
#define _REV_GAME_APPLICATION_FILESYSTEM_FILESYSTEM_H_

#include <functional>
#include <map>
#include <string>

#include <revCore/event/event.h>

namespace rev { namespace game {

	class FileSystem
	{
	public:
		typedef rev::Event<void, const char*>	FileEvent;

		FileEvent&	onFileChanged(const std::string& _fileName) { return mFileChangedEvents[_fileName]; }

	protected:
		std::map<std::string, FileEvent>	mFileChangedEvents;
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_APPLICATION_FILESYSTEM_FILESYSTEM_H_