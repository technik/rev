//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/15
//----------------------------------------------------------------------------------------------------------------------
// Keyboard controlled fly-by camera

#ifndef _REV_GAME_SCENE_CAMERA_FLYBYCAMERA_H_
#define _REV_GAME_SCENE_CAMERA_FLYBYCAMERA_H_

#include "camera.h"
#include <core/components/sceneNode.h>

namespace rev { namespace game {

	class SceneNode;

	class FlyByCamera : public Camera, public core::SceneNode
	{
	public:
		FlyByCamera(float _fov, float _aspectRatio, float _near, float _far);
		~FlyByCamera();

		void update(float _dt);
		void setSpeed(float _speed) { mSpeed = _speed; }

	private:
		float mSpeed;
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_CAMERA_FLYBYCAMERA_H_