////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, model processor
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 8th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Model loader interface

#ifndef _MODELPROCESSOR_LOADERS_MODELLOADER_H_
#define _MODELPROCESSOR_LOADERS_MODELLOADER_H_

// Forward declarations
namespace rev { namespace video { class CStaticModel; } }

class IModelLoader
{
public:
	ILoader() {}
	virtual ~ILoader() {}

	virtual rev::video::CStaticModel * modelFromFile	(const char * _filename) = 0;
};

#endif // _MODELPROCESSOR_LOADERS_MODELLOADER_H_