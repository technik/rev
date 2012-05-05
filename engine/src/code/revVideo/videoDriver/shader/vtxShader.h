////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Vertex shaders

#ifndef _REV_REVVIDEO_SHADER_VTXSHADER_H_
#define _REV_REVVIDEO_SHADER_VTXSHADER_H_

#include "revCore/resourceManager/managedResource.h"
#include "revCore/string.h"
#include <vector.h>

namespace rev { namespace video
{
	class CShader;

	class CVtxShader: public IManagedResource<CVtxShader, const char*, true>
	{
	public:
		// Constructor and destructor
		CVtxShader(const char* _name);
		~CVtxShader();

		// Other methods
		int				id() const	{	return mId;	}
		rtl::vector<CShader*>& users() { return mUsers; }
	private:
		int mId;
		rtl::vector<CShader*> mUsers;

		static rtl::vector<CShader*> tempUserStorage;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SHADER_VTXSHADER_H_
