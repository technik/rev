////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 29th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Pixel shaders

#ifndef _REV_REVVIDEO_SHADER_PXLSHADER_H_
#define _REV_REVVIDEO_SHADER_PXLSHADER_H_

#include <revCore/file/fileBinding.h>
#include <revCore/resourceManager/managedResource.h>
#include <revCore/string.h>
#include <vector.h>

namespace rev { namespace video
{
	// Forward declarations
	class CShader;

	class CPxlShader
		:public IManagedResource<CPxlShader, const char*, true>
		,private CRecreationFileBinding<CPxlShader>
	{
	public:
		CPxlShader(const char* _name);
		~CPxlShader();
		int		id() const	{	return mId;	}
		rtl::vector<CShader*>& users () { return mUsers; }

	private:
		void load(const char* _filename);

	private:
		CObjectDelegate<CPxlShader,const char*>* mPreShaderDelegate, *mPostShaderDelegate;
		string mName;
		int mId;
		rtl::vector<CShader*>	mUsers;	///< Shaders that use this pixel shader

		static rtl::vector<CShader*> tempUserStorage;
	};
}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SHADER_PXLSHADER_H_
