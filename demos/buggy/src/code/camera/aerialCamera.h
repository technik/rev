////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Buggy Demo
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// On November 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _BUGGYDEMO_CAMERA_AERIALCAMERA_H_
#define _BUGGYDEMO_CAMERA_AERIALCAMERA_H_

// -- Forward declarations
namespace rev {
	class CNode;

	namespace video
	{
		class CPerspectiveCamera;
	}	// namespace video
}	// namespace rev

namespace buggyDemo
{
	class CAerialCamera
	{
	public:
		// Constructor
		CAerialCamera(const rev::CNode * _target);
		~CAerialCamera();

		// Update
		void update ();

	private:
		const rev::CNode*						mTarget;
		rev::CNode *							mCamNode;
		class rev::video::CPerspectiveCamera*	mCamera;

	};

}	// namespace buggyDemo

#endif // _BUGGYDEMO_CAMERA_AERIALCAMERA_H_
