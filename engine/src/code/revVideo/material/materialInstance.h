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

	class CMaterialInstance
	{
	public:
		// Constructor and virtual destructor
		CMaterialInstance(IMaterial * _material);
		~CMaterialInstance();
		// Public interface
		const IMaterial *	material		() const;
		virtual	void		setEnvironment	() const {}
	private:
		IMaterial *	mMaterial;
	};
	
	//------------------------------------------------------------------------------------------------------------------
	inline const IMaterial * CMaterialInstance::material() const
	{
		return mMaterial;
	}

}	// namespace video
}	// namespace rev


#endif // _REV_REVVIDEO_MATERIAL_MATERIALINSTANCE_H_
