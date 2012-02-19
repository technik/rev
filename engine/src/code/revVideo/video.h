////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, video system
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// Video system

#ifndef _REV_REVVIDEO_VIDEO_H_
#define _REV_REVVIDEO_VIDEO_H_

namespace rev	{	namespace	video
{
	// Forward declarations
	class IVideoDriver;
	class IRenderer3d;

	// SVideo class
	class SVideo
	{
	public:
		//----------------------------------------------------------------------
		// Singleton life cycle
		//----------------------------------------------------------------------
		static	void	init		();			///< Init.
		static	void	end			();			///< End.
				void	update		();			///< Update.
		//----------------------------------------------------------------------
		// Singleton instance
		static	SVideo *get			();			///< Returns singleton instance.

		//----------------------------------------------------------------------
		// Video system interface
		//----------------------------------------------------------------------
		IVideoDriver *	driver		()	const;	///< Returs video driver.
		static IVideoDriver * getDriver() { return get()->driver(); }

	private:
		//----------------------------------------------------------------------
		// Constructor and destructor
		//----------------------------------------------------------------------
		SVideo();
		virtual	~SVideo();

		//----------------------------------------------------------------------
		// Internal methods
		//----------------------------------------------------------------------
		void	createDriver		();			///< Create the video driver.
		void	createRenderer		();			///< Create the video renderer

	private:
		//----------------------------------------------------------------------
		// Singleton instance
		static	SVideo *	s_pVideo;

		//----------------------------------------------------------------------
		// Video driver and renderer
		IVideoDriver*	m_pDriver;
		IRenderer3d	*	m_pRenderer;
	};

	//--------------------------------------------------------------------------
	inline SVideo * SVideo::get()
	{
		return s_pVideo;
	}

	//--------------------------------------------------------------------------
	inline IVideoDriver * SVideo::driver() const
	{
		return m_pDriver;
	}
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_VIDEO_H_
