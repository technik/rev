////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BUGGYDEMO_CAMERA_AERIALCAMERA_H_
#define _BUGGYDEMO_CAMERA_AERIALCAMERA_H_

// -- Forward declarations
namespace rev {	class CNode; }	// namespace rev

#include <revVideo/camera/perspectiveCamera.h>

namespace buggyDemo
{
	class CAerialCamera
	{
	public:
		// Constructor
		CAerialCamera(rev::CNode * _target);
		~CAerialCamera();

		// Update
		void update ();

		rev::video::CPerspectiveCamera*	camera() const { return mCamera; }

	private:
		void move();

	private:
		rev::CNode*						mTarget;
		rev::CNode *							mCamNode;
		class rev::video::CPerspectiveCamera*	mCamera;
	};

}	// namespace buggyDemo

#endif // _BUGGYDEMO_CAMERA_AERIALCAMERA_H_
