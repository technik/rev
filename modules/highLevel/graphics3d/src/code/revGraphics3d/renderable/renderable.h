//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/July/2
//----------------------------------------------------------------------------------------------------------------------
// base renderable type

#ifndef _REV_GRAPHICS3D_RENDERABLE_RENDERABLE_H_
#define _REV_GRAPHICS3D_RENDERABLE_RENDERABLE_H_

#include <cstdint>
#include <revMath/algebra/matrix.h>
#include <revVideo/types/image/image.h>

namespace rev { namespace graphics3d {

	class Renderable
	{
	public:
		Renderable	();
		~Renderable	();

		void	render			() const;

		void	setVertexData	(						///< Set new vertex data (deletes any previous data)
								uint16_t _count,		///< \param _count how many vertices
								math::Vec3f * _vertPos,	///< \param _vertPos vertex positions
								math::Vec3f * _vertNrm,	///< \param _vertNrm vertex normals
								math::Vec2f * _vertUV);	///< \param _vertUV vertex uv coordinates

		void	setFaceIndices	(						///< Set new triangled faces (deletes any previous faces)
								uint16_t _nIndices,		///< \param _count how many faces
								uint16_t * _indices,	///< \param _indices face indices
								bool _strip = false);	///< \param 
		void setTexture(const video::Image* _tex) { mTexture = _tex; }
		const video::Image* texture() const { return mTexture; }
		float boundingRadius() const {return mBoundingRadius;}
		float boundingSqRadius() const {return mBoundingSqRadius;}

		// Useful factories
		static Renderable* plane		(const math::Vec2f& _size);
		static Renderable* box			(const math::Vec3f& _size);
		static Renderable* geoSphere	(float _radius, uint16_t _nMeridians, uint16_t _nParallels);

	public:
		math::Mat34f	m;
		bool isVisible;

	private:
		math::Vec3f*	mVertices;
		uint16_t		mNVertices;
		math::Vec3f*	mNormals;
		math::Vec2f*	mUVs;
		uint16_t*		mTriangles;
		uint16_t		mNTriIndices;
		uint16_t*		mTriStrip;
		uint16_t		mStripLength;
		float			mBoundingRadius;
		float			mBoundingSqRadius;

		const video::Image*	mTexture;
	};

}	// namespace graphics3d
}	// namespace rev

#endif // _REV_GRAPHICS3D_RENDERABLE_RENDERABLE_H_