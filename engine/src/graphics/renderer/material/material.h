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
#include <graphics/backend/commandBuffer.h>
#include <graphics/backend/texture2d.h>
#include <math/algebra/vector.h>
#include <memory>
#include <string>
#include <vector>
#include "Effect.h"

namespace rev { namespace graphics {

	class Material
	{
	public:
		Effect& effect() { return *mEffect;}

		Material(const std::shared_ptr<Effect>& effect);

		// New params can only be added to the material before calling init
		void addParam(const std::string& name,float f);
		void addParam(const std::string& name, const math::Vec3f& v);
		void addParam(const std::string& name, const math::Vec4f& v);
		void addTexture(const std::string& name, gfx::Texture2d t);

		const std::string& bakedOptions() const { return mShaderOptionsCode; }
		void bindParams(gfx::CommandBuffer::UniformBucket& renderer) const;

	private:
		const std::shared_ptr<Effect> mEffect;
		std::string mShaderOptionsCode;

		std::vector<std::pair<GLint,float>>			mFloatParams;
		std::vector<std::pair<GLint,math::Vec3f>>	mVec3fParams;
		std::vector<std::pair<GLint,math::Vec4f>>	mVec4fParams;
		std::vector<std::pair<GLint,gfx::Texture2d>>	mTextureParams;
	};

}}
