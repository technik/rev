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
		bool		leftClickDown() const { return clickLeft; }

		// Windows specific interface
		bool processMessage(MSG message);

	private:
		math::Vec2i position;
		bool		clickLeft = false;

		static PointingInputWindows* sInstance;
	};

	using PointingInput = PointingInputWindows;
#endif // _WIN32

}}
