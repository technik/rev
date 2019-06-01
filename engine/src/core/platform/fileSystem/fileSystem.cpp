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
#include "fileSystem.h"
#include "file.h"
#include <cassert>
#ifdef ANDROID
#include <android/asset_manager.h>
#endif // ANDROID

namespace rev {
	namespace core {
#ifdef ANDROID
		void FileSystem::init(AAssetManager* _mgr) {
			File::setAssetMgr(_mgr);
		}

		void FileSystem::end() {
			// 
		}
#endif // ANDROID

#ifdef _WIN32
		//--------------------------------------------------------------------------------------------------------------
		FileSystem* FileSystem::sInstance = nullptr;

		//--------------------------------------------------------------------------------------------------------------
		FileSystem* FileSystem::get() {
			assert(sInstance);
			return sInstance;
		}

		//--------------------------------------------------------------------------------------------------------------
		void FileSystem::registerPath(const std::filesystem::path& path)
		{
			m_registedPaths.emplace_back(path);
		}

		//--------------------------------------------------------------------------------------------------------------
		File* FileSystem::readFile(const std::filesystem::path& filename) const
		{
			if (filename.is_relative())
			{
				// Try concatenating with all registered paths
				for (auto& path : m_registedPaths)
				{
					auto fullPath = path / filename;
					if (std::filesystem::exists(fullPath))
					{
						return new File(filename.generic_u8string().c_str());
					}
				}
			}
			// Finally, try just the unmodified path (either absolute or relative)
			if (std::filesystem::exists(filename))
			{
				return new File(filename.generic_u8string().c_str());
			}
			return nullptr;
		}

		//--------------------------------------------------------------------------------------------------------------
		void FileSystem::init() {
			assert(!sInstance);
			sInstance = new FileSystem;
		}

		//--------------------------------------------------------------------------------------------------------------
		void FileSystem::end() {
			assert(sInstance);
			delete sInstance;
			sInstance = nullptr;
		}
#endif // _WIN32

	} // namespace core
}	// namespace rev
