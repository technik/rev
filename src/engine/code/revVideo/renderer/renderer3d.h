////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on September 11th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// 3d renderer interface

#ifndef _REV_REVVIDEO_RENDERER_RENDERER3D_H_
#define _REV_REVVIDEO_RENDERER_RENDERER3D_H_

namespace rev { namespace video
{
	// Forward declarations
	class IVideoDriver;

	// Renderer 3d interface
	class IRenderer3d
	{
	public:
		// virtual destructor
		virtual ~IRenderer3d() {}
		// Interface
		virtual void	renderFrame	() = 0;
	};

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_RENDERER_RENDERER3D_H_

