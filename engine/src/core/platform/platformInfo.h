//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef ANDROID
#include <android/native_activity.h>
#endif // ANDROID

namespace rev {
	namespace core {

#ifdef ANDROID
		struct StartUpInfo {
			ANativeActivity* activity;
			ANativeWindow*	window;
		};

#else //!ANDROID
		struct StartUpInfo {
			int argC;
			const char** argV;
		};
#endif // !ANDROID

} }
