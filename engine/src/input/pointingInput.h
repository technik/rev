//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <math/algebra/vector.h>
#ifdef _WIN32
#include <Windows.h>
#endif

namespace rev { namespace input {

#ifdef _WIN32
	class PointingInputWindows
	{
	public:
		// Cross platform interface
		static void init();

		static PointingInputWindows* get() { return sInstance; }
		math::Vec2i touchPosition() const { return position; }
		bool		leftDown() const { return mLeftDown; }
		bool		middleDown() const { return mMiddleDown; }
		bool		rightDown() const { return mRightDown; }

		// Windows specific interface
		bool processMessage(MSG message);

	private:
		math::Vec2i position;
		bool		mLeftDown = false;
		bool		mMiddleDown = false;
		bool		mRightDown = false;

		static PointingInputWindows* sInstance;
	};

	using PointingInput = PointingInputWindows;
#endif // _WIN32

#ifdef ANDROID
	class PointingInputAndroid
	{
	public:
		// Cross platform interface
		static void init();

		static PointingInputAndroid* get() { return sInstance; }
		math::Vec2i touchPosition() const { return position; }
		bool		leftDown() const { return mTouch; }

		void touchDown() { mTouch = true;}
		void touchUp() { mTouch = false;}
		void move(const math::Vec2i& _pos) { position = _pos; }

	private:
		math::Vec2i position;
		bool		mTouch = false;

		static PointingInputAndroid* sInstance;
	};

	using PointingInput = PointingInputAndroid;
#endif // ANDROID

}}
