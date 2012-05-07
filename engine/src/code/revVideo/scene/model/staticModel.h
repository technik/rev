////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model

#ifndef _REV_REVVIDEO_SCENE_MODEL_STATICMODEL_H_
#define _REV_REVVIDEO_SCENE_MODEL_STATICMODEL_H_

#include "revCore/resourceManager/managedResource.h"
#include "revCore/string.h"
#include "revVideo/scene/renderable.h"

namespace rev { namespace video {

	// Forward declaration
	class CVtxShader;
	
	class CStaticModel: public IRenderable, public IManagedResource<CStaticModel, const char*, true>
	{
	public:
		// -- Constructors & virtual destructor ------------------------------------------------------------------------
		CStaticModel ();						///< Empty constructor.
		CStaticModel (const char* fileName);	///< Create model from filename.
		~CStaticModel();						///< Destructor

		// -- Rendering ------------------------------------------------------------------------------------------------
		void	setEnvironment					() const;	///< Adjust rendering enviroment to render this static model
		void	render							() const;	///< Render the model using current enviroment configuration

		// -- Accessor methods -----------------------------------------------------------------------------------------
		CVtxShader*				shader			() const;	///< Vertex shader used by this model
		unsigned short			nVertices		() const;	///< How many vertices the model has
		unsigned				nTriIndices		() const;	///< Number of independent triangle in the model * 3
		const float*			vertices		() const;	///< Return model's vertex array
		const float*			normals			() const;	///< Return model's normal array
		const float*			uvs				() const;	///< Return model's uv array
		const unsigned short*	triangles		() const;	///< Triangle faces indices

		void					setVertexData	(			///< Set new vertex data (deletes any previous data)
												unsigned short _count,	///< \param _count how many vertices
												float * _vertPos,		///< \param _vertPos vertex positions
												float * _vertNrm,		///< \param _vertNrm vertex normals
												float * _vertUV);		///< \param _vertUV vertex uv coordinates

		void					setFaceIndices	(			///< Set new triangled faces (deletes any previous faces)
												unsigned _nIndices,	///< \param _count how many faces
												unsigned short * _indices,	///< \param _indices face indices
												bool _strip = false);		///< \param 

	private:
		static CVtxShader * sShader;
	private:
		float*			mVertices;
		unsigned short	mNVertices;
		float*			mNormals;
		float*			mUVs;
		unsigned short*	mTriangles;
		unsigned		mNTriIndices;
		unsigned short*	mTriStrip;
		unsigned		mStripLength;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------
	inline CVtxShader*				CStaticModel::shader		() const	{	return sShader;		}
	inline unsigned short			CStaticModel::nVertices		() const	{	return mNVertices;	}
	inline unsigned					CStaticModel::nTriIndices	() const	{	return mNTriIndices;}
	inline const float*				CStaticModel::vertices		() const	{	return mVertices;	}
	inline const float*				CStaticModel::normals		() const	{	return mNormals;	}
	inline const float*				CStaticModel::uvs			() const	{	return mUVs;		}
	inline const unsigned short*	CStaticModel::triangles		() const	{	return mTriangles;	}

}	// namespace video
}	// namespace rev

#endif // _REV_REVVIDEO_SCENE_MODEL_STATICMODEL_H_
