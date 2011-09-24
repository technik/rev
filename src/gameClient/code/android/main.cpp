////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine game client
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 20th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Android interface between the Java Activity and the native Game Client
#ifdef ANDROID
// Java native interface
#include <jni.h>

// Engine header files
#include <revGame/gameClient/gameClient.h>

// Active namespaces
using namespace rev::game;

// Global data
CGameClient * g_gameClient = 0;

//----------------------------------------------------------------------------------------------------------------------
// Method declarations
//----------------------------------------------------------------------------------------------------------------------
extern "C" {
	JNIEXPORT void JNICALL Java_com_rev_gameclient_revRenderer_initGameClient(JNIEnv* _env, jobject _obj);
	JNIEXPORT void JNICALL Java_com_rev_gameclient_revRenderer_endGameClient(JNIEnv* _env, jobject _obj);
	JNIEXPORT void JNICALL Java_com_rev_gameclient_revRenderer_updateGameClient(JNIEnv* _env, jobject _obj);
	JNIEXPORT void JNICALL Java_com_rev_gameclient_revRenderer_setScreenSize(JNIEnv* _env, jobject _obj);
}

//----------------------------------------------------------------------------------------------------------------------
// Implementations
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_rev_gameclient_revRenderer_initGameClient(JNIEnv* /*_env*/, jobject /*_obj*/)
{
	g_gameClient = new CGameClient();
	g_gameClient->init();
}

//----------------------------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_rev_gameclient_revRenderer_endGameClient(JNIEnv* /*_env*/, jobject /*_obj*/)
{
	g_gameClient->end();
	delete g_gameClient;
}

//----------------------------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_rev_gameclient_revRenderer_updateGameClient(JNIEnv* /*_env*/, jobject /*_obj*/)
{
	g_gameClient->update();
}

//----------------------------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_rev_gameclient_revRenderer_setScreenSize(JNIEnv* /*_env*/, jobject /*_obj*/)
{
	//
}

#endif // ANDROID
