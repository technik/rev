//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <core/world/worldLayer.h>
#include <vector>

namespace rev {
	namespace video {

		class ForwardRenderer;
		class RenderObj;

		class RenderLayer : public core::WorldLayer {
		public:
			RenderLayer(ForwardRenderer*);
			void update(float _dt) override;

		private:
			std::vector<RenderObj*>	mObjects;
			ForwardRenderer*		mRenderer;
		};

} }	// namespace rev::video