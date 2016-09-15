//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/13
//----------------------------------------------------------------------------------------------------------------------
// Simple file
#ifndef _REV_CORE_PLATFORM_FILESYSTEM_FILE_H_
#define _REV_CORE_PLATFORM_FILESYSTEM_FILE_H_

#include <core/resources/namedResourceMgr.h>
#ifdef ANDROID
#include <android/asset_manager.h>
#endif // ANDROID


namespace rev {
	namespace core {
		
		class File : public NamedResource<File>
		{
		public:
			File(const std::string& _path); // File must exist
			~File();

			const void *	buffer		() const;
			const char *	bufferAsText() const;
			size_t			sizeInBytes	() const;


#ifdef ANDROID
			static void setAssetMgr( AAssetManager* _mgr );
#endif // ANDROID

		private:
			size_t		mSize = 0;
			void*		mBuffer = nullptr;
#if _DEBUG
			std::string	mPath;
#endif // _DEBUG
#ifdef ANDROID
			static AAssetManager* sAssetMgr;
#endif // ANDROID
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