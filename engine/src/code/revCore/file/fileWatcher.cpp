////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 10th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File watcher

#include "fileWatcher.h"

#include <revCore/codeTools/assert/assert.h>

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	SFileWatcher * SFileWatcher::sInstance = 0;

	//------------------------------------------------------------------------------------------------------------------
	void SFileWatcher::init()
	{
		revAssert(0 == sInstance, "Error: file watcher already initialized");
		sInstance = new SFileWatcher();
		revAssert(0 != sInstance, "Error: unable to create SFileWatcher");
	}

	//------------------------------------------------------------------------------------------------------------------
	void SFileWatcher::end()
	{
		revAssert(0 != sInstance, "Error: Can't destroy SFileWatcher, someone already did it");
		delete sInstance;
		sInstance = 0;
	}

	//------------------------------------------------------------------------------------------------------------------
	SFileWatcher* SFileWatcher::get()
	{
		return sInstance;
	}

	//------------------------------------------------------------------------------------------------------------------
	void SFileWatcher::addWatcher(const char * _fileName, IDelegate<const char*>* _watcher)
	{
		mWatchers[string(_fileName)].push_back(_watcher);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SFileWatcher::removeWatcher(IDelegate<const char*>* _watcher)
	{
		for(rtl::map<string,rtl::vector<IDelegate<const char*>*> >::iterator i = mWatchers.begin(); i != mWatchers.end(); ++i)
		{
			rtl::vector<IDelegate<const char*>*>& v = i->second;
			for(rtl::vector<IDelegate<const char*>*>::iterator j = v.begin(); j != v.end(); ++j)
			{
				if(*j == _watcher)
				{
					v.erase(j);
					return;
				}
			}
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void SFileWatcher::notifyFileChanged(const char * _fileName)
	{
		rtl::map<string,rtl::vector<IDelegate<const char*>*> >::iterator i = mWatchers.find(string(_fileName));
		if(i != mWatchers.end())
		{
			// Copy the vector instead of referencing it may get corrupted due to watchers deleting themselves during recreation
			rtl::vector<IDelegate<const char*>*> v = i->second;
			for(rtl::vector<IDelegate<const char*>*>::iterator j = v.begin(); j != v.end(); ++j)
			{
				(*j)->invoke(_fileName);
			}
		}
	}
}	// namespace rev