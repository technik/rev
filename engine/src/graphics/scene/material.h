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
#include <core/types/json.h>
#include <core/platform/fileSystem/file.h>
#include <graphics/driver/openGL/openGL.h>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/driver/texture.h>
#include <math/algebra/vector.h>
#include <memory>
#include <fstream>
#include <string>
#include <utility>
#include <vector>

namespace rev { namespace graphics {

	class Material
	{
	public:
		std::string name;

		void load(const std::string& fileName)
		{
			name = fileName;
			core::File inFile(fileName);
			if(inFile.buffer())
			{
				auto& in = inFile.asStream();
				clear();
				core::Json materialData;
				in >> materialData;
				for(auto& f : materialData["float"])
					addParam(f[0].get<GLint>(), f[1].get<float>());
				for(auto& f : materialData["tex"])
				{
					auto textureName = f[1].get<std::string>();
					auto texture = Texture::load(textureName);
					addTexture(f[0].get<GLint>(), texture);
				}
			}
		}

		void save()
		{
			if(name.empty())
				return;
			std::ofstream out(name);
			if(out.is_open())
			{
				core::Json data;
				auto& floatParams = data["float"];
				for(auto& f : mFloatParams)
					floatParams[0] = { f.first, f.second };
				auto& texParams = data["tex"];
				for(auto& f : mTextureParams)
					floatParams[0] = { f.first, f.second->name };
			}
		}

		void clear()
		{
			mFloatParams.clear();
			mVec3fParams.clear();
			mVec4fParams.clear();
			mTextureParams.clear();
		}

		float* floatParam(GLint i)
		{
			for(auto& p : mFloatParams)
				if(p.first == i)
					return &p.second;
			return nullptr;
		}

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

		void addTexture(GLint pos, std::shared_ptr<const Texture> t)
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
				glBindTexture(GL_TEXTURE_2D, textureParam.second->glName());
			}
		}

	private:
		using TexturePtr = std::shared_ptr<const Texture>;

		std::vector<std::pair<GLint,float>>			mFloatParams;
		std::vector<std::pair<GLint,math::Vec3f>>	mVec3fParams;
		std::vector<std::pair<GLint,math::Vec4f>>	mVec4fParams;
		std::vector<std::pair<GLint,TexturePtr>>	mTextureParams;
	};

}}
