////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 25th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Touch input for android

#ifdef ANDROID

#ifndef _REV_REVINPUT_TOUCHINPUT_ANDROID_TOUCHINPUTANDROID_H_
#define _REV_REVINPUT_TOUCHINPUT_ANDROID_TOUCHINPUTANDROID_H_

#include "revInput/touchInput/touchInput.h"

namespace rev { namespace input
{
	class CTouchInputAndroid: public STouchInput
	{
	public:
		unsigned	nMaxTouches	() const	{return 1;}
		bool		pressed		(unsigned /*_touchIdx = 0*/) const {return false;}
		bool		held		(unsigned /*_touchIdx = 0*/) const {return true;}
		bool		released	(unsigned /*_touchIdx = 0*/) const {return false;}

		CVec2		touchPos	(unsigned /*_touchIdx = 0*/) const {return CVec2::zero;}
	private:
		//
	};
}	// namespace input
}	// namespace rev

#endif // _REV_REVINPUT_TOUCHINPUT_ANDROID_TOUCHINPUTANDROID_H_

#endif // ANDROID
