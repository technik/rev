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

#include "FolderWatcher.h"

namespace fs = std::filesystem;

namespace rev::core {

	FolderWatcher::FolderWatcher(const path& path)
		: m_watchedPath(path)
	{
		// Initialize file list
		readFolderState(m_fileTimeStamps);
	}

	void FolderWatcher::listen(Listener&& listener) {
		m_listeners.push_back(std::forward < Listener>(listener));
	}

	void FolderWatcher::update() {
		buildChangeList();
		m_fileTimeStamps = std::move(m_currentFileList);
		
		if (m_changesList.empty())
			return;

		for(auto& listener : m_listeners)
		{
			listener(m_changesList);
		}
	}

	void FolderWatcher::readFolderState(std::vector<TimeStampedFile>& list)
	{
		list.clear();

		for(auto& file : fs::directory_iterator(m_watchedPath))
		{
			list.emplace_back(file.path(), file.last_write_time());
		}
	}

	void FolderWatcher::buildChangeList() {
		readFolderState(m_currentFileList);

		m_changesList.clear();
		for (auto& [filePath, lastTime] : m_fileTimeStamps)
		{
			auto& path = filePath; // Workaround for lambda capture
			auto newState = std::find_if(m_currentFileList.begin(), m_currentFileList.end(), [&, path](const auto& newEntry) {
				return newEntry.first == path && newEntry.second > lastTime;
				});
			if (newState != m_currentFileList.end())
			{
				m_changesList.push_back(path);
			}
		}
	}
}