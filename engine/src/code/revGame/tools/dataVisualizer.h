////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, time
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on March 14th, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Data Visualizer

#ifndef _REV_GAME_TOOLS_DATAVISUALIZER_H_
#define _REV_GAME_TOOLS_DATAVISUALIZER_H_

#include <revCore/math/vector.h>
#include <revCore/types.h>
#include <revVideo/color/color.h>
#include <revVideo/material/material.h>
#include <revVideo/scene/renderable.h>
#include <revVideo/scene/renderableInstance.h>
#include <vector.h>

namespace rev { namespace game
{
	class CDataVisualizer : public video::IRenderableInstance
	{
	public:
		enum EZeroAlignment
		{
			eNone,
			eTop,
			eCenter,
			eBottom,
		};
	public:
		// Life cycle
		CDataVisualizer(const CVec2& _size, unsigned _capacity = 100, EZeroAlignment _align = eCenter);
		~CDataVisualizer();

		// Channels management
		int		addChannel		(const video::CColor& _clr, float _min, float _max);

		// Data
		void	resetChannel	(int _channel, float _value = 0.f);
		void	pushData		(int _channel, float _data);

		// Visual
		void	setMargins		(const CVec2& _margins);

	private:
		class CChannel
		{
		public:
			CChannel(const video::CColor& _color, unsigned _capacity, float _min, float _max, const CVec2& _canvasSize);

					CVec3*			data	()			{ return mData.data();		}
			const	unsigned short*	indices	() const	{ return mIndices.data();	}

			void	reset	(float _resetValue);
			float	normalizeData	(float _data)	{ return mFactor * _data + mOffset; }

		private:
			float	mFactor;
			float	mOffset;
			video::CColor	mColor;
			rtl::vector<CVec3>			mData;
			rtl::vector<unsigned short>	mIndices;
		};

		class CRenderable : public video::IRenderable
		{
		public:
			CRenderable(const CVec2& _size, const rtl::vector<CChannel*>& _channels);
			~CRenderable();

		private:
			CRenderable& operator=(const CRenderable&) {}

			typedef rtl::vector<CChannel*>	channelArrayT;

			video::CVtxShader * shader	() const;
			void		setEnviroment	() const;
			void		render			() const;

			CVec2	mSize;
			video::CVtxShader * mShader;
			CVec3*	mBGVertices;
			unsigned short * mIndices;
			const channelArrayT& mChannels;
		};

		class CMaterial : public video::IMaterial
		{
		public:
			CMaterial();
			~CMaterial();

			bool	usesAlpha		() const { return true; }
		private:
			void	setEnvironment	() const;
		};

	private:
		CRenderable*			mRenderable;
		CVec2					mSize;
		unsigned				mCapacity;
		rtl::vector<CChannel*>	mChannels;
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_TOOLS_DATAVISUALIZER_H_