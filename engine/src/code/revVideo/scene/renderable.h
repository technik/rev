////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 2nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// renderable

#ifndef _REV_REVVIDEO_SCENE_RENDERABLE_H_
#define _REV_REVVIDEO_SCENE_RENDERABLE_H_

namespace rev { namespace video
{
	class CVtxShader;

	class IRenderable
	{
	public:
		virtual ~IRenderable() {}

		virtual CVtxShader * shader	() const = 0;
		virtual void setEnvironment	() const = 0;
		virtual void render			() const = 0;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_RENDERABLE_H_
