////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 14th, 2012
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
	CDataVisualizer::CDataVisualizer(const CVec2& _size, unsigned _capacity, EZeroAlignment _align)
		:mSize(_size)
		,mCapacity(_capacity)
	{
		mRenderable = new CRenderable(_size, mChannels);
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
	int CDataVisualizer::addChannel(const video::CColor& _clr, float _min, float _max)
	{
		
		mChannels.push_back(new CChannel(_clr, mCapacity, _min, _max, mSize));
		return int(mChannels.size()) - 1;
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::resetChannel(int _channelId, float _data)
	{
		revAssert(_channelId > 0 && unsigned(_channelId) < mChannels.size() );
		mChannels[_channelId]->reset(_data);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::pushData(int _channelId, float _data)
	{
		revAssert(_channelId >= 0 && unsigned(_channelId) < mChannels.size() );
		unsigned i = 0;
		CChannel& channel = *(mChannels[_channelId]);
		CVec3 * data = channel.data();
		for(; i < mCapacity-1; ++i)
		{
			data[i].z = data[i+1].z;
		}
		data[i].z = channel.normalizeData(_data);
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::setMargins(const CVec2& _margins)
	{
		_margins;
	}

	//------------------------------------------------------------------------------------------------------------------
	CDataVisualizer::CChannel::CChannel(const video::CColor& _color, unsigned _capacity, float _min, float _max,
		const CVec2& _canvasSize)
		:mColor(_color)
	{
		mFactor = _canvasSize.y / (_max-_min);
		mOffset = ((_max+_min) * 0.5f) * mFactor + (_canvasSize.y * 0.5f);
		for (unsigned short i = 0; i < _capacity; ++i)
		{
			mData.push_back(CVec3(0.f+(_canvasSize.x / (_capacity-1)) * i, 0.f, _canvasSize.y * 0.5f));
			mIndices.push_back(i);
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::CChannel::reset(float _value)
	{
		for(unsigned i = 0; i < mData.size(); ++i)
		{
			mData[i].z = mOffset + _value * mFactor;
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	CDataVisualizer::CRenderable::CRenderable(const CVec2& _size, const rtl::vector<CChannel*>& _channels)
		:mSize(_size)
		,mChannels(_channels)
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
	void CDataVisualizer::CRenderable::setEnvironment() const
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void CDataVisualizer::CRenderable::render() const
	{
		IVideoDriver * driver = SVideo::getDriver();
		// Draw background
		driver->setRealAttribBuffer(IVideoDriver::eVertex, 3, mBGVertices);
		int colorUniform = driver->getUniformId("color");
		driver->setUniform(colorUniform, CColor(1.f,1.f,1.f,0.2f));
		driver->drawIndexBuffer(4, mIndices, IVideoDriver::eTriStrip);
		// Draw data channels
		for(unsigned i = 0; i < mChannels.size(); ++i)
		{
			driver->setRealAttribBuffer(IVideoDriver::eVertex, 3, mChannels[i]->data());
			driver->setUniform(colorUniform, CColor::RED);
			driver->drawIndexBuffer(100, mChannels[i]->indices(), IVideoDriver::eLineStrip);
		}
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