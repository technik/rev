//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On October 27th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Basic canvas application

#ifndef _REV_CANVAS_APPLICATION_CANVASAPPLICATION_H_
#define _REV_CANVAS_APPLICATION_CANVASAPPLICATION_H_

#include <revCore/event/event.h>

namespace rev {
	namespace video {
		class VideoDriver;
		class Driver3d;
	}
}

namespace rev { namespace canvas
{
	class Canvas;

	class CanvasApplication
	{
	public:
		CanvasApplication				();
		~CanvasApplication				();
		Event<>&		onUpdate	();

		bool				update		();

	protected:
		Canvas*				canvas		();

	private:
		virtual void draw() {} // Override this function with your main loop code
		void initializeVideoSystem();

	private:
		Event<>			mOnUpdate;
		// Driver cache
		video::VideoDriver*	mVideoDriver;
		video::Driver3d*	mDriver3d;
		Canvas*				mCanvas;
	};

	//----------------------------------------------------------------------------
	// Inline implementation
	//----------------------------------------------------------------------------
	inline Canvas* CanvasApplication::canvas()
	{
		return mCanvas;
	}

	//----------------------------------------------------------------------------
	inline Event<>& CanvasApplication::onUpdate()
	{
		return mOnUpdate;
	}

}	// namespace canvas
}	// namespace rev

#endif // _REV_CANVAS_APPLICATION_CANVASAPPLICATION_H_
