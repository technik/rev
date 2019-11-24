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

namespace rev::gfx {

	class Material
	{
	public:
		// A material parameter will only be bound in when it shares
		// binding flags with those passed to the bindParams method
		enum Flags
		{
			None = 0,
			Shading = 1<<0,
			Normals = 1 << 1,
			Emissive = 1 << 2,
			AlphaBlend = 1<<3,
			AlphaMask = 1<<4
		};

		template<typename T>
		struct Param
		{
			std::string name;
			T value;
			Flags flags = None;
		};

		enum Transparency
		{
			Opaque,
			Mask,
			Blend
		};

		struct Descriptor
		{
			const std::shared_ptr<Effect>& effect;
			std::vector<Param<float>> floatParams;
			std::vector<Param<math::Vec3f>> vec3Params;
			std::vector<Param<math::Vec4f>> vec4Params;
			std::vector<Param<Texture2d>> textures;
			Transparency transparency = Transparency::Opaque;
		};

		Effect& effect() const { return *mEffect; }

		Material(const Descriptor&);
		Transparency transparency() const { return mTransparency; }
		Flags flags() const { return mFlags; }
		bool isEmissive() const { return mFlags & Flags::Emissive; }

		const std::string& bakedOptions() const { return mShaderOptionsCode; }
		void bindParams(gfx::CommandBuffer::UniformBucket& dst, Flags) const;

	private:
		const std::shared_ptr<Effect> mEffect;
		Transparency mTransparency;
		Flags mFlags;
		std::string mShaderOptionsCode;

		template<class T> struct BindingParam
		{
			GLint location;
			T value;
			Flags flags;
		};

		template<class T>
		std::vector<BindingParam<T>> loadParams(const std::vector<Param<T>>& descParams);

		std::vector<BindingParam<float>>		mFloatParams;
		std::vector<BindingParam<math::Vec3f>>	mVec3fParams;
		std::vector<BindingParam<math::Vec4f>>	mVec4fParams;
		std::vector<BindingParam<Texture2d>>	mTextureParams;
	};

	//------------------------------------------------------------------------------------
	inline static Material::Flags operator|(Material::Flags a, Material::Flags b)
	{
		return static_cast<Material::Flags>(static_cast<int>(a) | static_cast<int>(b));
	}

}
