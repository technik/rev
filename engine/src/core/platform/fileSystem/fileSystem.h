//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/18
//----------------------------------------------------------------------------------------------------------------------
// File system interface
#ifndef _REV_CORE_PLATFORM_FILESYSTEM_FILESYSTEM_H_
#define _REV_CORE_PLATFORM_FILESYSTEM_FILESYSTEM_H_

#ifdef _WIN32
#include "windows/fileSystemWindows.h"
#endif // _WIN32
#ifdef ANDROID
#include <android/asset_manager.h>
#endif // ANDROID

#include "file.h"
#include <experimental/filesystem>
#include <fstream>

namespace rev {
	namespace core {
#ifdef _WIN32
		class FileSystem : public FileSystemBase {
#else
		class FileSystem {
#endif
		public:
#ifdef ANDROID
			static void init(AAssetManager* _mgr);
			static void end();
#endif // ANDROID
#ifdef _WIN32
			static void init();
			static void end();
			static FileSystem*	get(); // Must be implemented in derived class' cpp.
			FileEvent&			onFileChanged(const std::string& _fileName) { return mFileChangedEvents[_fileName]; }
#endif // _WIN32

			// Paths, folders and files
			File* openFile(const std::string& _filePath) {
				std::string path = findPath(_filePath);
				if(!path.empty())
					return new File(path);
				return nullptr;
			}

			std::string findPath(const std::string& _path) {
				if(std::experimental::filesystem::exists(_path))
					return _path;
				else if(!mLocalPathStack.empty())
				{
					std::string localFile = mLocalPathStack.back()+_path;
					if(std::experimental::filesystem::exists(localFile))
						return localFile;
				}
				return "";
			}

			std::ifstream openStream(const std::string& _filePath) {
				return std::ifstream(findPath(_filePath));
			}
			void pushLocalPath(const std::string& _path) { mLocalPathStack.push_back(_path); }
			void popLocalPath() { mLocalPathStack.pop_back(); }

			static std::string extractFileFolder(const std::string& _fileName) {
				auto fullPath = std::experimental::filesystem::path(_fileName);
				auto folder = fullPath.remove_filename();
				return folder.string();
			}

		private:
			std::vector<std::string> mLocalPathStack;
			FileSystem() = default;
			~FileSystem() = default;
#ifdef _WIN32
			static FileSystem*	sInstance;
#endif // _WIN32
		};
	}
}	// namespace rev

#endif // _REV_CORE_PLATFORM_FILESYSTEM_FILESYSTEM_H_