////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 14th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Visualizer

#include "dataVisualizer.h"

#include <revCore/node/node.h>
#include <revCore/resourceManager/resourceManager.h>
#include <revVideo/video.h>
#include <revVideo/videoDriver/shader/pxlShader.h>
#include <revVideo/videoDriver/shader/vtxShader.h>
#include <revVideo/videoDriver/videoDriver.h>

using namespace rev::video;

namespace rev { namespace game
{
	//------------------------------------------------------------------------------------------------------------------
	CDataVisualizer::CDataVisualizer(const CVec2& _size, EZeroAlignment _align)
	{
		mRenderable = new CRenderable(_size);
		setRenderable(mRenderable);
		setMaterial(new CMaterial());
		attachTo(new CNode());
		_align;
	}

	//------------------------------------------------------------------------------------------------------------------
	CDataVisualizer::~CDataVisualizer()
	{
		setRenderable(0);
		delete mRenderable;
		delete node();
	}

	//------------------------------------------------------------------------------------------------------------------
	int CDataVisualizer::addChannel(const video::CColor& _clr, float _min, float _max, float _offset)
	{
		_clr;
		_min;
		_max;
		_offset;
		return -1;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::deleteChannel(int _channelId)
	{
		_channelId;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::resetChannel(int _channelId, float _data)
	{
		_channelId;
		_data;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::pushData(int _channelId, float _data)
	{
		_channelId;
		_data;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::setMargins(const CVec2& _margins)
	{
		_margins;
	}

	//------------------------------------------------------------------------------------------------------------------
	CDataVisualizer::CRenderable::CRenderable(const CVec2& _size):mSize(_size)
	{
		mShader = CVtxShader::manager()->get("dataGraph.vtx");
		mBGVertices = new CVec3[4];
		mBGVertices[0] = CVec3(0.f,0.f,0.f);
		mBGVertices[1] = CVec3(0.f,0.f,mSize.y);
		mBGVertices[2] = CVec3(mSize.x,0.f,mSize.y);
		mBGVertices[3] = CVec3(mSize.x,0.f,0.f);
		mIndices = new unsigned short[4];
		mIndices[0] = 0;
		mIndices[1] = 1;
		mIndices[2] = 3;
		mIndices[3] = 2;
	}

	//------------------------------------------------------------------------------------------------------------------
	CDataVisualizer::CRenderable::~CRenderable()
	{
		CVtxShader::manager()->release(mShader);
		delete mIndices;
		delete mBGVertices;
	}

	//------------------------------------------------------------------------------------------------------------------
	CVtxShader * CDataVisualizer::CRenderable::shader() const
	{
		return mShader;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::CRenderable::setEnviroment() const
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::CRenderable::render() const
	{
		IVideoDriver * driver = SVideo::getDriver();
		driver->setRealAttribBuffer(IVideoDriver::eVertex, 3, mBGVertices);
		int colorUniform = driver->getUniformId("color");
		driver->setUniform(colorUniform, CColor::WHITE);
		driver->drawIndexBuffer(4, mIndices, IVideoDriver::eTriStrip);
	}

	//------------------------------------------------------------------------------------------------------------------
	CDataVisualizer::CMaterial::CMaterial()
	{
		mShader = CPxlShader::manager()->get("dataGraph.pxl");
	}

	//------------------------------------------------------------------------------------------------------------------
	CDataVisualizer::CMaterial::~CMaterial()
	{
		CPxlShader::manager()->release(mShader);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::CMaterial::setEnvironment() const
	{
	}

}	// namespace game
}	// namespace rev