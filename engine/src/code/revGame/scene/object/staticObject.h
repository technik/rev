////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, cameras
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on February 19th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static object

#ifndef _REV_GAME_SCENE_OBJECT_STATICOBJECT_H_
#define _REV_GAME_SCENE_OBJECT_STATICOBJECT_H_

#include "revCore/math/vector.h"

// Forward declarations
namespace rev {
	class CNode;

	namespace video 
	{
		class CStaticModelInstance;
	}
}

namespace rev { namespace game
{
	class CStaticObject
	{
	public:
		CStaticObject(const char * _model, const char * _material, const CVec3& _pos = CVec3::zero);
		CStaticObject(const CStaticObject& _other); // Copy constructor

	private:
		CNode * mNode;
		video::CStaticModelInstance * mModel;
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_OBJECT_STATICOBJECT_H_
