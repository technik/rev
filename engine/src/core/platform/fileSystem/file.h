//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// Simple file
#ifndef _REV_CORE_PLATFORM_FILESYSTEM_FILE_H_
#define _REV_CORE_PLATFORM_FILESYSTEM_FILE_H_

#include <core/resources/resourceManager.h>
#include <core/resources/creator/virtualConstructor.h>
#include <core/resources/ownership/refLink.h>

namespace rev {
	namespace core {
		
		class File : public ManagedResource<std::string, File, VirtualConstructor<File, std::string>, RefLink>
		{
		public:
			File(const std::string& _path); // File must exist
			~File();

			static File* openExisting(const std::string& _path); // Returns nullptr if the file doesn't exist already

			bool			readAll		();
			void			setContent	(const void* _buffer, size_t, bool _hardCopy = true);

			const void *	buffer		() const;
			const char *	bufferAsText() const;
			size_t			sizeInBytes	() const;

		private:
			size_t		mSize = 0;
			const void*	mBuffer = nullptr;
			bool		mMustWrite = false;
			std::string	mPath;
		};

		//------------------------------------------------------------------------------------------------------------------
		// Inline implementation
		//------------------------------------------------------------------------------------------------------------------
		inline const void * File::buffer		() const { return mBuffer; }
		inline const char * File::bufferAsText	() const { return reinterpret_cast<const char*>(mBuffer); }
		inline size_t		File::sizeInBytes	() const { return mSize; }
	}
}

#endif // _REV_CORE_PLATFORM_FILESYSTEM_FILE_H_