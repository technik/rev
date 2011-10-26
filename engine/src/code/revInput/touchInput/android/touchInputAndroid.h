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
		// Constructor
		CTouchInputAndroid();

		// Inherited interface
		unsigned	nMaxTouches	() const	{return 2;}
		bool		pressed		(unsigned _touchIdx = 0) const;
		bool		held		(unsigned _touchIdx = 0) const;
		bool		released	(unsigned _touchIdx = 0) const;

		CVec2		touchPos	(unsigned /*_touchIdx = 0*/) const {return CVec2::zero;}

		// Platform specific interface
		void		touchPress	(int _id, const CVec2& _pos);
		void		touchMove	(int _id, const CVec2& _pos);
		void		touchRelease(int _id, const CVec2& _pos);

	private:
		bool		mPressed	[2];
		bool		mHeld		[2];
		bool		mReleased	[2];

		int			mTouchId	[2];
		CVec2		mTouchPos	[2];
	};
}	// namespace input
}	// namespace rev

#endif // _REV_REVINPUT_TOUCHINPUT_ANDROID_TOUCHINPUTANDROID_H_

#endif // ANDROID
