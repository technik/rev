////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex shaders

#ifndef _REV_REVVIDEO_SHADER_VTXSHADER_H_
#define _REV_REVVIDEO_SHADER_VTXSHADER_H_

#include "revCore/resourceManager/resource.h"
#include "revCore/string.h"

namespace rev { namespace video
{
	class CVtxShader: public TResource<CVtxShader, string>
	{
	public:
		// Constructor
		CVtxShader(const string& _name);

		// Other methods
		int				id() const	{	return mId;	}
	private:
		int mId;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SHADER_VTXSHADER_H_
