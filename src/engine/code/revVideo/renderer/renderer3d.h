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
		// constructor & virtual destructor
		IRenderer3d();
		virtual ~IRenderer3d() {}
		// Interface
		virtual void	renderFrame	() = 0;
		void	setDriver	(IVideoDriver * _driver);

	protected:
		IVideoDriver * driver	() const;

	private:
		IVideoDriver * mDriver;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline methods
	//------------------------------------------------------------------------------------------------------------------
	inline IRenderer3d::IRenderer3d():
		mDriver(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void IRenderer3d::setDriver(IVideoDriver* _driver)
	{
		mDriver = _driver;
	}

	//------------------------------------------------------------------------------------------------------------------
	inline IVideoDriver* IRenderer3d::driver() const
	{
		return mDriver;
	}

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_RENDERER_RENDERER3D_H_

