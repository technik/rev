////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fern�ndez-Ag�era Tortosa (a.k.a. Technik)
// Created on February 19th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Material

#include "material.h"

#include <revCore/codeTools/assert/assert.h>

namespace rev 
{
	IManagedResource<video::IMaterial,const char*,false>::TManager * video::IMaterial::sManager = 0;

}	// namespace rev