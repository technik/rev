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
		CDataVisualizer(const CVec2& _size, EZeroAlignment _align = eCenter);
		~CDataVisualizer();

		// Channels management
		int		addChannel		(const video::CColor& _clr, TReal _min, TReal _max, TReal _offset);
		void	deleteChannel	(int _channelId);

		// Data
		void	resetChannel	(int _channel, float _value = 0.f);
		void	pushData		(int _channel, float _data);

		// Visual
		void	setMargins		(const CVec2& _margins);

	private:

	private:
		class CRenderable : public video::IRenderable
		{
		public:
			CRenderable(const CVec2& _size);
			~CRenderable();

		private:
			video::CVtxShader * shader	() const;
			void		setEnviroment	() const;
			void		render			() const;

			CVec2	mSize;
			video::CVtxShader * mShader;
			CVec3*	mBGVertices;
			unsigned short * mIndices;
		};

		CRenderable*	mRenderable;

		class CMaterial : public video::IMaterial
		{
		public:
			CMaterial();
			~CMaterial();
		private:
			void	setEnvironment	() const;
		};
	};
}	// namespace game
}	// namespace rev

#endif // _REV_GAME_TOOLS_DATAVISUALIZER_H_