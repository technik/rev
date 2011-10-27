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

#include "revCore/codeTools/log/log.h"

namespace rev { namespace input
{
	//------------------------------------------------------------------------------------------------------------------
	CTouchInputAndroid::CTouchInputAndroid()
	{
		mPressed[0] = false;
		mPressed[1] = false;
		mHeld[0] = false;
		mHeld[1] = false;
		mReleased[0] = false;
		mReleased[1] = false;
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CTouchInputAndroid::pressed(unsigned _touchIdx) const
	{
		return mPressed[_touchIdx];
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CTouchInputAndroid::held(unsigned _touchIdx) const
	{
		return mHeld[_touchIdx];
	}

	//------------------------------------------------------------------------------------------------------------------
	bool CTouchInputAndroid::released(unsigned _touchIdx) const
	{
		return mHeld[_touchIdx];
	}

	//------------------------------------------------------------------------------------------------------------------
	CVec2 CTouchInputAndroid::touchPos(unsigned _touchIdx) const
	{
		return mTouchPos[_touchIdx];
	}

	//------------------------------------------------------------------------------------------------------------------
	void CTouchInputAndroid::touchPress(int _id, const CVec2& _pos)
	{
		// LOG_ANDROID("TouchPress( %d, CVec2(%f, %f)", _id, _pos.x, _pos.y);
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
		mTouchId[idx] = _id;
		mPressed[idx] = true;
		mTouchPos[idx] = _pos;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CTouchInputAndroid::touchMove(int _id, const CVec2& _pos)
	{
		// LOG_ANDROID("TouchMove( %d, CVec2(%f, %f)", _id, _pos.x, _pos.y);
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
		// LOG_ANDROID("TouchRelease( %d, CVec2(%f, %f)", _id, _pos.x, _pos.y);
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
		mReleased[_idx] = true;
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
							jint _id, jfloat _x, jfloat _y)
{
	rev::input::CTouchInputAndroid * input = static_cast<rev::input::CTouchInputAndroid*>(rev::input::STouchInput::get());
	input->touchPress(_id, rev::CVec2(_x, _y));
}

//----------------------------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_rev_gameclient_revView_nativeTouchMove(JNIEnv* /*_env*/, jobject /*_obj*/,
							jint _id, jfloat _x, jfloat _y)
{
	rev::input::CTouchInputAndroid * input = static_cast<rev::input::CTouchInputAndroid*>(rev::input::STouchInput::get());
	input->touchMove(_id, rev::CVec2(_x, _y));
}

//----------------------------------------------------------------------------------------------------------------------
JNIEXPORT void JNICALL Java_com_rev_gameclient_revView_nativeTouchRelease(JNIEnv* /*_env*/, jobject /*_obj*/,
							jint _id, jfloat _x, jfloat _y)
{
	rev::input::CTouchInputAndroid * input = static_cast<rev::input::CTouchInputAndroid*>(rev::input::STouchInput::get());
	input->touchRelease(_id, rev::CVec2(_x, _y));
}

#endif // ANDROID
