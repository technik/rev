////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shaders

#ifndef _REV_REVVIDEO_VIDEODRIVER_SHADER_SHADER_H_
#define _REV_REVVIDEO_VIDEODRIVER_SHADER_SHADER_H_

#include "revCore/resourceManager/resource.h"
#include "revCore/resourceManager/resourceManager.h"
#include "rtl/pair.h"

using rtl::pair;

namespace rev { namespace video
{
	class CVtxShader;
	class CPxlShader;

	class CShader: public TResource<CShader, pair<CVtxShader*,CPxlShader*> >
	{
	public:
		typedef TResourceManager<CShader, pair<CVtxShader*,CPxlShader*> > managerT;

	public:
		static	managerT*	manager();
		CShader(pair<CVtxShader*,CPxlShader*> _baseShaders);
		void	setEnviroment() const;
	private:
		// Shader manager
		static managerT * sManager;
		
		CVtxShader * mVtx;
		CPxlShader * mPxl;
		int		mId;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_VIDEODRIVER_SHADER_SHADER_H_
