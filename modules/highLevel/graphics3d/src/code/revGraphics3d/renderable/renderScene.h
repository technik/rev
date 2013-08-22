//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// render scene

#ifndef _REV_GRAPHICS3D_RENDERABLE_RENDERSCENE_H_
#define _REV_GRAPHICS3D_RENDERABLE_RENDERSCENE_H_

#include <vector>
#include <functional>

namespace rev { namespace graphics3d {

	class Renderable;

	class RenderScene
	{
	public:
		void			traverse		(std::function<void (const Renderable*)>) const;
		virtual void	traverse		(std::function<void (const Renderable*)>, std::function<bool (const Renderable*)> _filter) const = 0;
	};


	class LinearRenderScene
	{
	public:
		void	traverse		(std::function<void (const Renderable*)>, std::function<bool (const Renderable*)> _filter) const;

		void	add				(const Renderable*);
		void	remove			(const Renderable*);

	private:
		std::vector<const Renderable*>	mRenderQueue;
	};

}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GRAPHICS3D_RENDERABLE_RENDERSCENE_H_