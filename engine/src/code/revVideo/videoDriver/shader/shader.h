////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shaders

#ifndef _REV_REVVIDEO_VIDEODRIVER_SHADER_SHADER_H_
#define _REV_REVVIDEO_VIDEODRIVER_SHADER_SHADER_H_

#include "revCore/resourceManager/resource.h"
#include <vector.h>

using rtl::pair;

namespace rev { namespace video
{
	class CVtxShader;
	class CPxlShader;

	class CShader: public TResource<CShader, pair<CVtxShader*,CPxlShader*> >
	{
	public:
		CShader(pair<CVtxShader*,CPxlShader*> _baseShaders);
		~CShader();
		void	setEnvironment() const;

		void	refresh();
	private:
		void unregisterAsUser(rtl::vector<CShader*>& _userList);
		CVtxShader * mVtx;
		CPxlShader * mPxl;
		int		mId;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_VIDEODRIVER_SHADER_SHADER_H_
