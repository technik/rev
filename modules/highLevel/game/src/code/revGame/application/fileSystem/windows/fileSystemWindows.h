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

#include <Windows.h>

namespace rev { namespace game {

	class FileSystemWindows : FileSystem
	{
	public:
		FileSystemWindows();

		bool 
	};

}	// namespace game
}	// namespace rev

#endif // _WIN32

#endif // _REV_GAME_APPLICATION_FILESYSTEM_FILESYSTEM_WINDOWS_H_