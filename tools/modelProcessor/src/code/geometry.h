////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, model processor
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on November 20th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Geometry data

#ifndef _MODELPROCESSOR_VERTEXINFO_H_
#define _MODELPROCESSOR_VERTEXINFO_H_

// Engine headers
#include "revCore/math/vector.h"
#include "vector.h"

namespace rev { namespace video {
	class CStaticModel;
}	// namespace video
}	// namespace rev

namespace modelProcessor
{
	struct TVertexInfo
	{
	public:
		unsigned short mVIdx;	///< Vertex index
		unsigned short mNIdx;	///< Normal index
		unsigned short mTIdx;	///< Texture coordinate index

		// Public constructors
		TVertexInfo() {}
		TVertexInfo(unsigned short _vIdx,
					unsigned short _nIdx,
					unsigned short _tIdx)
					:mVIdx(_vIdx)
					,mNIdx(_nIdx)
					,mTIdx(_tIdx) {}
		~TVertexInfo() {}

		// Comparison operator
		bool	operator == (const TVertexInfo& _other) const;
	};

	class CGeometry
	{
	public:
		void addVertexPos	( const rev::CVec3& _pos);
		void addNormal		( const rev::CVec3& _nrm);
		void addTexCoord	( const rev::CVec2& _tc);
		void addFace		( const TVertexInfo& _v1, const TVertexInfo& _v2, const TVertexInfo& _v3);

		void fillModel		( rev::video::CStaticModel * _model ) const;

	private:
		unsigned short vertexIndex ( const TVertexInfo& _vtx);

	private:
		rtl::vector<rev::CVec3>		mVPositions;
		rtl::vector<rev::CVec3>		mNormals;
		rtl::vector<rev::CVec2>		mTexCoord;
		rtl::vector<TVertexInfo>	mVtxQueue;
		rtl::vector<unsigned short>	mFaceIndices;
	};

	//------------------------------------------------------------------------------------------------------------------
	inline bool TVertexInfo::operator==(const TVertexInfo& _other) const
	{
		return (mVIdx==_other.mVIdx) && (mNIdx==_other.mNIdx) && (mTIdx==_other.mTIdx);
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CGeometry::addVertexPos ( const rev::CVec3& _pos )
	{
		mVPositions.push_back(_pos);
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CGeometry::addNormal ( const rev::CVec3& _nrm )
	{
		mNormals.push_back(_nrm);
	}

	//------------------------------------------------------------------------------------------------------------------
	inline void CGeometry::addTexCoord ( const rev::CVec2& _tc )
	{
		mTexCoord.push_back(_tc);
	}

}	// namespace modelProcessor

#endif // _MODELPROCESSOR_VERTEXINFO_H_

/*

faces are triples of indices.

Faces are read as triplets of vertex infos

Each vertex info is searched in a queue. If it's found, it's index is returned.
If not found, a new vertex is added and the index returned.

Those 3 indices are used to build the new face and this is added to the 

*/