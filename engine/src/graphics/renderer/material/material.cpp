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
#include "Material.h"
#include <graphics/backend/openGL/openGL.h>

using namespace std;
using namespace rev::math;

namespace rev::gfx {

	//----------------------------------------------------------------------------------------------
	Material::Material(const Descriptor& desc)
		: mEffect(desc.effect)
		, mFlags(Flags::None)
	{
		// Params
		mFloatParams = loadParams(desc.floatParams);
		mVec3fParams = loadParams(desc.vec3Params);
		mVec4fParams = loadParams(desc.vec4Params);
		mTextureParams = loadParams(desc.textures);

#ifdef _DEBUG
		for (auto& t : mTextureParams)
		{
			assert(t.value.isValid());
		}
#endif

		// Transparency
		mTransparency = desc.transparency;
	}

	//----------------------------------------------------------------------------------------------
	void Material::bindParams(gfx::CommandBuffer::UniformBucket& renderer, Flags flags) const
	{
		for (const auto& v : mFloatParams)
		{
			if(v.flags & flags)
				renderer.addParam(v.location, v.value);
		}
		for(const auto& v : mVec3fParams)
		{
			if (v.flags & flags)
				renderer.addParam(v.location, v.value);
		}
		for(const auto& v : mVec4fParams)
		{
			if (v.flags & flags)
				renderer.addParam(v.location, v.value);
		}
		for(const auto& t : mTextureParams)
		{
			if (t.flags & flags)
				renderer.addParam(t.location, t.value);
		}
	}

	//----------------------------------------------------------------------------------------------
	template<class T>
	auto Material::loadParams(const std::vector<Param<T>>& descParams)->std::vector<BindingParam<T>>
	{
		std::vector<BindingParam<T>> loadedParams;
		loadedParams.reserve(descParams.size());

		for (auto& srcParam : descParams)
		{
			BindingParam<T> dstParam;
			auto effectProperty = mEffect->property(srcParam.name);
			dstParam.location = effectProperty->location;
			dstParam.value = srcParam.value;
			dstParam.flags = srcParam.flags;
			mFlags = mFlags | srcParam.flags;

			mShaderOptionsCode += effectProperty->preprocessorDirective();

			loadedParams.push_back(dstParam);
		}

		return loadedParams;
	}
}