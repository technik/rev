////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 27th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Material instance

#ifndef _REV_REVVIDEO_MATERIAL_MATERIALINSTANCE_H_
#define _REV_REVVIDEO_MATERIAL_MATERIALINSTANCE_H_

namespace rev { namespace video
{
	class IMaterial;

	class IMaterialInstance
	{
	public:
		// Constructor and virtual destructor
		IMaterialInstance(const IMaterial * _material):mMaterial(_material) {}
		~IMaterialInstance() {}
		// Public interface
		const IMaterial *	material		() const;
		virtual	void		setEnviroment	() const {}
	private:
		const IMaterial *	mMaterial;
	};
	
	//------------------------------------------------------------------------------------------------------------------
	inline const IMaterial * IMaterialInstance::material() const
	{
		return mMaterial;
	}

}	// namespace video
}	// namespace rev


#endif // _REV_REVVIDEO_MATERIAL_MATERIALINSTANCE_H_
