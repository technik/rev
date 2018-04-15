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
#include <graphics/driver/openGL/openGL.h>

using namespace std;
using namespace rev::math;

namespace rev { namespace graphics {

	//----------------------------------------------------------------------------------------------
	Material::Material(const shared_ptr<Effect> _effect)
		: mEffect(_effect)
	{
	}

	//----------------------------------------------------------------------------------------------
	void Material::addParam(const string& name, float f)
	{
		auto prop = mEffect->property(name);
		if(prop)
		{
			mShaderOptionsCode += prop->preprocessorDirective();
			mFloatParams.emplace_back(prop->location, f);
		}
	}

	//----------------------------------------------------------------------------------------------
	void Material::addParam(const string& name, const Vec3f& v)
	{
		auto prop = mEffect->property(name);
		if(prop)
		{
			mShaderOptionsCode += prop->preprocessorDirective();
			mVec3fParams.emplace_back(prop->location, v);
		}
	}

	//----------------------------------------------------------------------------------------------
	void Material::addParam(const string& name, const Vec4f& v)
	{
		auto prop = mEffect->property(name);
		if(prop)
		{
			mShaderOptionsCode += prop->preprocessorDirective();
			mVec4fParams.emplace_back(prop->location, v);
		}
	}

	//----------------------------------------------------------------------------------------------
	void Material::addTexture(const string& name, TexturePtr t)
	{
		auto prop = mEffect->property(name);
		if(prop)
		{
			mShaderOptionsCode += prop->preprocessorDirective();
			mTextureParams.emplace_back(prop->location, t);
		}
	}

	//----------------------------------------------------------------------------------------------
	void Material::bindParams(BackEndRenderer::Command& cmd) const
	{
		cmd.mFloatParams.insert(cmd.mFloatParams.end(), mFloatParams.begin(), mFloatParams.end());
		cmd.mVec3fParams.insert(cmd.mVec3fParams.end(), mVec3fParams.begin(), mVec3fParams.end());
		cmd.mVec4fParams.insert(cmd.mVec4fParams.end(), mVec4fParams.begin(), mVec4fParams.end());
		for(const auto& t : mTextureParams)
		{
			cmd.mTextureParams.emplace_back(t.first, t.second.get());
		}
	}

}}
