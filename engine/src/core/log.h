//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#ifdef ANDROID
#include <android/log.h>
#endif

namespace rev { namespace core {
	struct Log {
		static void info(const char* _msg) {
			(void)__android_log_print(ANDROID_LOG_INFO, "rev.AndroidPlayer", _msg);
		}
		template<class T_>
		static void info(const char* _msg, T_ _data) {
			(void)__android_log_print(ANDROID_LOG_INFO, "rev.AndroidPlayer", _msg, _data);
		}
		static void debug(const char* _msg) {
			(void)__android_log_print(ANDROID_LOG_DEBUG, "rev.AndroidPlayer", _msg);
		}
		static void warning(const char* _msg) {
			(void)__android_log_print(ANDROID_LOG_WARN, "rev.AndroidPlayer", _msg);
		}
		static void error(const char* _msg) {
			(void)__android_log_print(ANDROID_LOG_ERROR, "rev.AndroidPlayer", _msg);
		}
	};
}}
