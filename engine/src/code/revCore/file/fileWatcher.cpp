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
}	// namespace rev