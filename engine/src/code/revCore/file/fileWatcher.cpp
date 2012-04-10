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
		codeTools::revAssert(0 == sInstance);
		sInstance = new SFileWatcher();
		codeTools::revAssert(0 != sInstance);
	}

	//------------------------------------------------------------------------------------------------------------------
	void SFileWatcher::end()
	{
		codeTools::revAssert(0 != sInstance);
		delete sInstance;
		sInstance = 0;
	}
}	// namespace rev