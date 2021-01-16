//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2018 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#pragma once
#include "component.h"
#include <memory>
#include <graphics/scene/renderObj.h>

namespace rev { namespace game {

	class Transform;

	class MeshRenderer : public Component {
	public:
		MeshRenderer(
			const std::shared_ptr<gfx::RenderObj>& renderObj
		);

		~MeshRenderer()
		{
			
		}

		void init		() override;
		void update		(float _dt) override;

		const	gfx::RenderObj& renderObj() const { return *mRenderable; }
				gfx::RenderObj& renderObj() { return *mRenderable; }

	private:
		const std::shared_ptr<gfx::RenderObj>	mRenderable;
		Transform*								mSrcTransform = nullptr;
	};

}}	// namespace rev::game