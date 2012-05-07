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
		if(!_file || *_file=='\0')
		{
			mDelegate = 0;
			return;
		}
		mDelegate = new CObjectDelegate<IFileBinding,const char*>(this, &IFileBinding::reload);
		SFileWatcher::get()->addWatcher(_file, mDelegate);
	}

	//------------------------------------------------------------------------------------------------------------------
	IFileBinding::~IFileBinding()
	{
		if( 0 == mDelegate)
			return;
		SFileWatcher::get()->removeWatcher(mDelegate);
		delete mDelegate;
	}

}	// namespace rev
