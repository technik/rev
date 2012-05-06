////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on May 6th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File binding

#include "fileBinding.h"

#include "fileWatcher.h"
#include <revCore/delegate/delegate.h>

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	IFileBinding::IFileBinding(const char * _file)
	{
		mDelegate = new CObjectDelegate<IFileBinding,const char*>(this, &IFileBinding::reload);
		SFileWatcher::get()->addWatcher(_file, mDelegate);
	}

	//------------------------------------------------------------------------------------------------------------------
	IFileBinding::~IFileBinding()
	{
		SFileWatcher::get()->removeWatcher(mDelegate);
		delete mDelegate;
	}

}	// namespace rev
