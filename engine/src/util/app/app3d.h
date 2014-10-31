//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa
// on 2014-10-31
//----------------------------------------------------------------------------------------------------------------------
// Base class for 3d applications
#ifndef _REV_UTIL_APP_APP3D_H_
#define _REV_UTIL_APP_APP3D_H_

namespace rev {

	class App3d {
	public:
		App3d();
		App3d(int _argc, const char** _argv);

		bool update();

	protected:
		virtual bool frame(float _dt); // Usually override this
	private:
		void preFrame();
		void postFrame();
	};

}	// namespace rev

#endif // _REV_UTIL_APP_APP3D_H_