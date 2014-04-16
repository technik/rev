//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/15
//----------------------------------------------------------------------------------------------------------------------
// Scene graph node

#ifndef _REV_GAME_SCENE_SCENEGRAPH_SCENENODE_H_
#define _REV_GAME_SCENE_SCENEGRAPH_SCENENODE_H_

#include "transformSrc.h"

#include <math/algebra/vector.h>

namespace rev { namespace game {

	class SceneNode : public TransformSrc
	{
	public:
		// Accessors
		void			move				(const math::Vec3f& _translation);
		void			setPos				(const math::Vec3f& _position);
		void			rotate				(const math::Vec3f& _axis, float _angle);
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_SCENEGRAPH_SCENENODE_H_
