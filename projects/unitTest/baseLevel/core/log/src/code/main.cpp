//------------------------------------------------------------------------
// Rev Project
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On February 7th, 2013
//------------------------------------------------------------------------
// Log unit test
#include <android_native_app_glue.h>

#include <revCore/codeTools/log/log.h>

void android_main(struct android_app* state)
{
	// Make sure glue isn't stripped.
    app_dummy();
	
	rev::revLog() << "Hello from rev::Log!\n";
	while(1);
}