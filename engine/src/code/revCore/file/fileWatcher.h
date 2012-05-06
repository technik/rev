////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 10th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File watcher
// File watcher raises events to registered delegates whenever a watched file is modified.

#ifndef _REV_CORE_FILE_FILEWATCHER_H_
#define _REV_CORE_FILE_FILEWATCHER_H_

#include <revCore/delegate/delegate.h>
#include <revCore/string.h>
#include <rtl/map.h>
#include <vector.h>

namespace rev
{
	class SFileWatcher
	{
	public:
		// Singleton interface
		static void init();
		static void end();
		static SFileWatcher * get();

		// File watcher interface
		// Once you're watching a file, the delegate will be invoked everytime the file gets modified.
		void addWatcher(const char * _filename, IDelegate<const char*> * _watcher);
		void removeWatcher(IDelegate<const char*> * _watcher);

		// Call this to notity the watcher that a given file has changed
		void notifyFileChanged(const char * _filename);

	private:
		SFileWatcher() {}
		~SFileWatcher(){}
		static SFileWatcher * sInstance;

	private:
		// Each watched file has an associated list of delegates to call on modification
		rtl::map<string, rtl::vector<IDelegate<const char*>*> >	mWatchers;
	};
}	// namespace rev

#endif // _REV_CORE_FILE_FILEWATCHER_H_