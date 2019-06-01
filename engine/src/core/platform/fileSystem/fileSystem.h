//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2019 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
//----------------------------------------------------------------------------------------------------------------------
// File system interface
#pragma once

#include <filesystem>
#include <vector>

#ifdef _WIN32
#include "windows/fileSystemWindows.h"
#endif // _WIN32
#ifdef ANDROID
#include <android/asset_manager.h>
#endif // ANDROID

namespace rev {
	namespace core {

		class File;

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
			static FileSystem*	get();

			using Path = std::filesystem::path;

			void registerPath(const Path& path);
			// Locate an existing path within the registered folders or, if it doesn't exist, return an empty path.
			Path resolvePath(const Path&) const;
			// Returns nullptr if the file doesn't exist in any of the registered paths, or the working directory
			File* readFile(const Path& filename) const;

			// TODO: Scoped filesystem access with temporary sets of overriding paths?

#endif // _WIN32
		private:
			FileSystem() = default;
			~FileSystem() = default;

			std::vector<std::filesystem::path> m_registedPaths;

#ifdef _WIN32
			static FileSystem*	sInstance;
#endif // _WIN32
		};
	}
}	// namespace rev