//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#include <cjson/json.h>
#include <core/world/worldLayer.h>
#include <vector>

namespace rev {
	namespace video {

		class ForwardRenderer;
		class RenderObj;

		class RenderLayer : public core::WorldLayer {
		public:
			RenderLayer(ForwardRenderer& _r) : mRenderer (_r) {}
			bool update(float _dt) override;

			RenderObj* createRenderObj(const cjson::Json&);

		private:
			std::vector<RenderObj*>	mObjects;
			ForwardRenderer&		mRenderer;
		};

} }	// namespace rev::video