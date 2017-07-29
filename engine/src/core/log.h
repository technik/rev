//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/March/30
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#ifdef ANDROID
#include <android/log.h>
#else
#include <iostream>
#endif

namespace rev { namespace core {
	struct Log {
#ifdef ANDROID
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
#else
		static void info(const char* _msg) {
			std::cout << "Info: " << _msg << "\n";
		}
		template<class T_>
		static void info(const char* _msg, T_ _data) {
			std::cout << "Info: " << _msg << _data << "\n";
		}
		static void debug(const char* _msg) {
			std::cout << "Debug: " << _msg << "\n";
		}
		static void warning(const char* _msg) {
			std::cout << "Warning: " << _msg << "\n";
		}
		static void error(const char* _msg) {
			std::cout << "Error: " << _msg << "\n";
		}
#endif
	};
}}
