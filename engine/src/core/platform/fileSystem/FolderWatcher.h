//----------------------------------------------------------------------------------------------------------------------
// Copyright 2020 Carmelo J Fdez-Aguera
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

#pragma once

#include <chrono>
#include <filesystem>
#include <functional>
#include <string>
#include <vector>

namespace rev::core {

	// Notify listeners when files change in a watched folder.
	class FolderWatcher
	{
	public:
		using path = std::filesystem::path;
		using time = std::filesystem::file_time_type;

		FolderWatcher(const path& path);

		using Listener = std::function<void(const std::vector<path>&)>;
		void listen(Listener&&);

		void update();

	private:
		using TimeStampedFile = std::pair<path, time>;

		void readFolderState(std::vector<TimeStampedFile>& dst);
		void buildChangeList();

		std::filesystem::path m_watchedPath;
		std::vector<TimeStampedFile> m_fileTimeStamps;
		std::vector<Listener> m_listeners;

		std::vector<path> m_changesList;
		std::vector<TimeStampedFile> m_currentFileList; // Stored as a member to avoid allocating memory all the time
	};

}	// namespace rev::core