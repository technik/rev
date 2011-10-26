////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 26th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Touch input for android

#ifdef ANDROID
// Java native interface
#include <jni.h>

#include "touchInputAndroid.h"

namespace rev { namespace input
{
	//------------------------------------------------------------------------------------------------------------------
	bool CTouchInputAndroid::pressed(unsigned _touchIdx)
	{
		return mPressed[_touchIdx];
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CTouchInputAndroid::held(unsigned _touchIdx)
	{
		return mHeld[_touchIdx];
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CTouchInputAndroid::released(unsigned _touchIdx)
	{
		return mHeld[_touchIdx];
	}

	//------------------------------------------------------------------------------------------------------------------
	CVec2 CTouchInputAndroid::touchPos(unsigned _touchIdx)
	{
		return mTouchPos[_touchIdx];
	}

	//------------------------------------------------------------------------------------------------------------------
	void CTouchInputAndroid::touchPress(int _id, const CVec2& _pos)
	{
		// Retrieve a valid index for this click
		int idx;
		if(!(mPressed[0] || mHeld[0]))
		{
			idx = 0;
		}else
		if(!(mPressed[1] || mHeld[1]))
		{
			idx = 1;
		}else
			return; // No available slot to process this touch
		mTouchId[_idx] = _id;
		mPressed[_idx] = true;
		mTouchPos[_idx] = _pos;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CTouchInputAndroid::touchMove(int _id, const CVec2& _pos)
	{
		if(mTouchId[0] == _id)
		{
			mTouchPos[0] = _pos;
		}else if(mTouchId[1] == _id)
		{
			mTouchPos[1] = _pos;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CTouchInputAndroid::touchRelease(int _id, const CVec2& _pos)
	{
		int _idx;
		if(mTouchId[0] == _id)
		{
			_idx = 0;
		}else if(mTouchId[1] == _id)
		{
			_idx = 1;
		}else
			return;

		mTouchPos[_idx] = _pos;
		mRelease[_idx] = true;
		mHeld[_idx] = false;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CTouchInputAndroid::refresh()
	{
		for(unsigned i = 0; i < 2; ++i)
		{
			mHeld[i] = mHeld[i] || mPressed[i];
			mReleased[i] = false;
			mPressed[i] = false;
		}
	}

}	// namespace input
}	// namespace rev

//----------------------------------------------------------------------------------------------------------------------
// JNI Implementation
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Method declarations
//----------------------------------------------------------------------------------------------------------------------
extern "C" {
	JNIEXPORT void JNICALL Java_com_rev_gameclient_revView_nativeTouchPress(JNIEnv* _env, jobject _obj,
							jint _id, jfloat _x, jfloat _y);
	JNIEXPORT void JNICALL Java_com_rev_gameclient_revView_nativeTouchMove(JNIEnv* _env, jobject _obj,
							jint _id, jfloat _x, jfloat _y);
	JNIEXPORT void JNICALL Java_com_rev_gameclient_revView_nativeTouchRelease(JNIEnv* _env, jobject _obj,
							jint _id, jfloat _x, jfloat _y);
}

//----------------------------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_rev_gameclient_revView_nativeTouchPress(JNIEnv* /*_env*/, jobject /*_obj*/,
							jint _id, jfloat _x, jfloat _y);
{
	CTouchInputAndroid * input = static_cast<CTouchInputAndroid*>(STouchInput::get());
	input->touchPress(_id, CVec2(_x, _y));
}

//----------------------------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_rev_gameclient_revView_nativeTouchMove(JNIEnv* /*_env*/, jobject /*_obj*/,
							jint _id, jfloat _x, jfloat _y);
{
	CTouchInputAndroid * input = static_cast<CTouchInputAndroid*>(STouchInput::get());
	input->touchMove(_id, CVec2(_x, _y));
}

//----------------------------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_rev_gameclient_revView_nativeTouchPress(JNIEnv* /*_env*/, jobject /*_obj*/,
							jint _id, jfloat _x, jfloat _y);
{
	CTouchInputAndroid * input = static_cast<CTouchInputAndroid*>(STouchInput::get());
	input->touchMove(_id, CVec2(_x, _y));
}

#endif // ANDROID
