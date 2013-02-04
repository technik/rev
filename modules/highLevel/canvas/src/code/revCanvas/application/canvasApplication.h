//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On October 27th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Basic canvas application

#ifndef _REV_CANVAS_APPLICATION_CANVASAPPLICATION_H_
#define _REV_CANVAS_APPLICATION_CANVASAPPLICATION_H_

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
		CanvasApplication();
		~CanvasApplication();

		bool update();

	protected:
		Canvas* canvas();

	private:
		virtual void draw() {} // Override this function with your main loop code
		void initializeVideoSystem();

	private:
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

}	// namespace canvas
}	// namespace rev

#endif // _REV_CANVAS_APPLICATION_CANVASAPPLICATION_H_
