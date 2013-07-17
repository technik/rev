//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On 2013/July/15
//----------------------------------------------------------------------------------------------------------------------
// Keyboard controlled fly-by camera

#ifndef _REV_GAME_SCENE_CAMERA_FLYBYCAMERA_H_
#define _REV_GAME_SCENE_CAMERA_FLYBYCAMERA_H_

#include <revGraphics3d/camera/camera.h>

namespace rev { namespace game {

	class SceneNode;

	class FlyByCamera
	{
	public:
		FlyByCamera(float _fov, float _aspectRatio, float _near, float _far);
		~FlyByCamera();

		void update();
		const graphics3d::Camera& cam() const { return mCam; }

	private:
		SceneNode* mNode;
		graphics3d::Camera	mCam;
	};

}	// namespace game
}	// namespace rev

#endif // _REV_GAME_SCENE_CAMERA_FLYBYCAMERA_H_