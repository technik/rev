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

#include <graphics/driver/texture.h>
#include <map>
#include <memory>
#include <string>

namespace rev { namespace game {

	class TextureManager {
	public:
		void init()
		{
			mErrorTexture = std::make_shared<graphics::Texture>(graphics::ImageRGB8::proceduralXOR(512));
		}

		std::shared_ptr<graphics::Texture>	get(const std::string& _name)
		{
			std::shared_ptr<graphics::Texture>	result = mErrorTexture; // By default, return error
			auto it = mLoadedTextures.find(_name);
			if(it == mLoadedTextures.end() || it->second.expired())
			{
				auto newTexture = graphics::Texture::load(_name);
				mLoadedTextures.insert(std::make_pair(_name, newTexture));
				return newTexture;
			} 
			else
			{
				result = it->second.lock();
			}

			return result;
		}

	private:
		std::shared_ptr<graphics::Texture>	mErrorTexture;
		std::map<std::string, std::weak_ptr<graphics::Texture>> mLoadedTextures;
	};

}}
