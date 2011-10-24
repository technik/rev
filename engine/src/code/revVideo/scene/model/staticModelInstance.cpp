////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on October 3rd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Static model instance

#include "staticModelInstance.h"
#include "staticModel.h"

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::CStaticModelInstance(CStaticModel * _model):
		mModel(_model)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModelInstance::~CStaticModelInstance()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	float * CStaticModelInstance::vertices() const
	{
		return mModel->vertices();
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned short * CStaticModelInstance::triangles() const
	{
		return mModel->triangles();
	}

	//------------------------------------------------------------------------------------------------------------------
	int	CStaticModelInstance::nTriangles() const
	{
		return mModel->nTriangles();
	}

	//------------------------------------------------------------------------------------------------------------------
	unsigned short * CStaticModelInstance::triStrip() const
	{
		return mModel->triStrip();
	}

	//------------------------------------------------------------------------------------------------------------------
	int CStaticModelInstance::stripLength() const
	{
		return mModel->stripLength();
	}
}	// namespace video
}	// namespace rev