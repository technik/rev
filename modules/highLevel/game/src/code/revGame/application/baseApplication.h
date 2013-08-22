//----------------------------------------------------------------------------------------------------------------------
// Game module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/28
//----------------------------------------------------------------------------------------------------------------------
// Basic application using rev game

#ifndef _REV_GAME_APPLICATION_BASEAPPLICATION_H_
#define _REV_GAME_APPLICATION_BASEAPPLICATION_H_

#include <cstdint>
#include <revGraphics3d/camera/camera.h>

namespace rev { namespace video {
	class VideoDriver;
	class Driver3d;
	class Shader;
}}	// video, rev

namespace rev { namespace graphics3d { class Renderer; }}

namespace rev { namespace game {

	class BaseApplication
	{
	public:
		BaseApplication();
		~BaseApplication();

		void run();

		void setCam		(const rev::graphics3d::Camera* _cam) { mCamera = _cam; }

	private:
		virtual bool update() { return true; }
		virtual void renderScene(const rev::graphics3d::Camera*) {}

		bool render ();

	protected:
		video::VideoDriver*				mVideoDriver;
		video::Driver3d*				mDriver3d;
		rev::graphics3d::Renderer*		mRenderer;
		const rev::graphics3d::Camera*	mCamera;
		math::Vec3f						mCamPos;
	};

}	// namespace game
}	// namespace rev


#endif // _REV_GAME_APPLICATION_BASEAPPLICATION_H_