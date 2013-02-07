//------------------------------------------------------------------------
// Rev Project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On February 7th, 2013
//------------------------------------------------------------------------
// Log unit test
#include <jni.h>
#include <errno.h>

#include <android/log.h>
#include <android_native_app_glue.h>

void android_main(struct android_app* state)
{
	// Make sure glue isn't stripped.
    app_dummy();
	
	__android_log_print(ANDROID_LOG_INFO, "native-activity", "HELLO!!!\n");
	while(1);
}