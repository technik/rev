//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// base application

#ifndef _REV_GRAPHICS3D_APPLICATION_APPLICATION3D_H_
#define _REV_GRAPHICS3D_APPLICATION_APPLICATION3D_H_

namespace rev { namespace video {
	class VideoDriver;
	class Driver3d;
	class Shader;
}}	// video, rev

namespace rev { namespace graphics3d {

	class Application3d
	{
	public:
		Application3d	();
		~Application3d	();
		void run		();

	private:
		virtual bool update() { return true; }

		void render ();

	private:
		video::VideoDriver* mVideoDriver;
		video::Driver3d*	mDriver3d;

		video::Shader*		mBasicShader;
	};

}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GRAPHICS3D_APPLICATION_APPLICATION3D_H_
