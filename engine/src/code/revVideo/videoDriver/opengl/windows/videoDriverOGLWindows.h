////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, linux video driver
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Windows openGL video Driver

#ifdef WIN32

#include "revCore/math/vector.h"
#include "revVideo/videoDriver/opengl/videoDriverOpenGL.h"

namespace rev { namespace video
{
	class CVideoDriverOGLWindows: public IVideoDriverOpenGL
	{
	public:
		//--------------------------------------------------------------------------------------------------------------
		// Constructor and destructor
		//--------------------------------------------------------------------------------------------------------------
		CVideoDriverOGLWindows	();
		~CVideoDriverOGLWindows	();

		//--------------------------------------------------------------------------------------------------------------
		// Per frame tasks
		void	endFrame	();

		//--------------------------------------------------------------------------------------------------------------
		// Accessor methods
		void	setScreenSize		(const unsigned int _width, const unsigned int _height);
		const CVec2& screenSize		() const;

	private:
		//--------------------------------------------------------------------------------------------------------------
		// For internal use
		void	createOpenGLWindow(const unsigned int _width, const unsigned int _height);
		CVec2	mScreenSize;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------
	inline const CVec2& CVideoDriverOGLWindows::screenSize() const
	{
		return mScreenSize;
	}

}	// namespace video
}	// namespace rev

#endif
