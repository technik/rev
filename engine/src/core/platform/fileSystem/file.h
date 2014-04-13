//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// Simple file
#ifndef _REV_CORE_PLATFORM_FILESYSTEM_FILE_H_
#define _REV_CORE_PLATFORM_FILESYSTEM_FILE_H_

#include <core/types.h>
#include "stdFile.h"

namespace rev {
	namespace core {
		class File : public StdFile {
		public:
			File(const string& _path)
				:StdFile(_path) {}
		};
	}
}

#endif // _REV_CORE_PLATFORM_FILESYSTEM_FILE_H_