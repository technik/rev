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
#include <graphics/driver/openGL/openGL.h>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <math/algebra/vector.h>
#include <utility>
#include <vector>

namespace rev { namespace graphics {

	class Material
	{
	public:

		void addParam(GLint pos,float f)
		{
			mFloatParams.push_back(std::make_pair(pos,f));
		}

		void addParam(GLint pos, const math::Vec3f& v)
		{
			mVec3fParams.push_back(std::make_pair(pos,v));
		}

		void addParam(GLint pos, const math::Vec4f& v)
		{
			mVec4fParams.push_back(std::make_pair(pos,v));
		}

		void addTexture(GLint pos, GLuint t)
		{
			mTextureParams.push_back(std::make_pair(pos,t));
		}

		void bind(GraphicsDriverGL& driver) const
		{
			for(const auto& f : mFloatParams)
				driver.bindUniform(f.first, f.second);
			for(const auto& v : mVec3fParams)
				driver.bindUniform(v.first, v.second);
			for(const auto& v : mVec4fParams)
				driver.bindUniform(v.first, v.second);
			for(GLenum t = 0; t < mTextureParams.size(); ++t)
			{
				auto& textureParam = mTextureParams[t];
				glUniform1i(textureParam.first, t);
				glActiveTexture(GL_TEXTURE0+t);
				glBindTexture(GL_TEXTURE_2D, textureParam.second);
			}
		}

	private:
		std::vector<std::pair<GLint,float>>			mFloatParams;
		std::vector<std::pair<GLint,math::Vec3f>>	mVec3fParams;
		std::vector<std::pair<GLint,math::Vec4f>>	mVec4fParams;
		std::vector<std::pair<GLint,GLuint>>		mTextureParams;
	};

}}
