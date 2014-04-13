//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// File implementation for platforms conforming to C++ standard
#ifndef _REV_CORE_PLATFORM_FILESYSTEM_STDFILE_H_
#define _REV_CORE_PLATFORM_FILESYSTEM_STDFILE_H_

#include <core/types.h>

namespace rev {
	namespace core {

		class StdFile {
		public:
			const void *	buffer		() const;
			const char *	bufferAsText() const;
			int				sizeInBytes	() const;

		protected:
			StdFile(const string& _path);
			~StdFile();

		private:
			unsigned	mSize = 0;
			void*		mBuffer = nullptr;
		};

		typedef StdFile FileBase;

		//------------------------------------------------------------------------------------------------------------------
		// Inline implementation
		//------------------------------------------------------------------------------------------------------------------
		inline const void * StdFile::buffer() const { return mBuffer; }
		inline const char * StdFile::bufferAsText() const { return reinterpret_cast<const char*>(mBuffer); }
		inline int			StdFile::sizeInBytes() const { return mSize; }
	}
}

#endif // _REV_CORE_PLATFORM_FILESYSTEM_STDFILE_H_