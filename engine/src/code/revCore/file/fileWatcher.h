////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 10th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File watcher
// File watcher raises events to registered delegates whenever a watched file is modified.

#ifndef _REV_CORE_FILE_FILEWATCHER_H_
#define _REV_CORE_FILE_FILEWATCHER_H_

#include <dictionary.h>

namespace rev
{
	// Forward declaration
	class IDelegate;

	class SFileWatcher
	{
	public:
		// Singleton interface
		static void init();
		static void end();
		static SFileWatcher * get();

		// File watcher interface
		// Once you're watching a file, the delegate will be invoked everytime the file gets modified.
		void watchFile(const char * _filename, IDelegate * delegate);
		void stopWatchingFile(const char * _filename);

		// Call this to notity the watcher that a given file has changed
		void notifyFileChanged(const char * _filename);

	private:
		SFileWatcher();
		~SFileWatcher();
		static SFileWatcher * sInstance;

	private:
		// Each watched file has an associated list of delegates to call on modification
		rtl::dictionary<vector<IDelegate*>, 64>	mWatchedFiles;
	};
}	// namespace rev

#endif // _REV_CORE_FILE_FILEWATCHER_H_