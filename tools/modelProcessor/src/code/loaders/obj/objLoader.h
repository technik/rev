////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, model processor
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 8th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Obj Model loader interface

#ifndef _MODELPROCESSOR_LOADERS_OBJ_OBJLOADER_H_
#define _MODELPROCESSOR_LOADERS_OBJ_OBJLOADER_H_

#include <loaders/modelLoader.h>

class CObjLoader : public IModelLoader
{
public:
	rev::video::CStaticModel * modelFromFile	(const char * _filename);
};

#endif // _MODELPROCESSOR_LOADERS_OBJ_OBJLOADER_H_