////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// File system

#ifndef _REV_REVCORE_FILE_FILE_H_
#define _REV_REVCORE_FILE_FILE_H_

#include "revCore/resourceManager/resource.h"
#include "revCore/string.h"

namespace rev {

class CFile : public TResource<CFile, string>
{
public:
	// Constructor and destructor
	CFile(const string& _name);
	~CFile();

	const void *	buffer		() const { return mBuffer;	}
	const char *	textBuffer	() const { return reinterpret_cast<const char*>(mBuffer); }
	int				size		() const { return mSize;	}

private:
	string	mFileName;
	int		mSize;
	void *	mBuffer;
};

}	// namespace rev

#endif // _REV_REVCORE_FILE_FILE_H_
