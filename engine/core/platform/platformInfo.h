//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef ANDROID
#include <android/native_activity.h>
#endif // ANDROID

namespace rev {
	namespace core {

#ifdef ANDROID
		struct PlatformInfo {
			ANativeWindow* window;
		};
#endif // ANDROID

#ifdef _WIN32
		struct PlatformInfo {
			int argC;
			const char** argV;
		};
#endif // !ANDROID

} }
