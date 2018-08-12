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

#include <core/tools/log.h>
#include <graphics/driver/openGL/openGL.h>
#include <graphics/driver/openGL/GraphicsDriverOpenGL.h>
#include <graphics/Image.h>
#include <string>

namespace rev { namespace graphics {

	class Texture
	{
	public:
		std::string name;

		Texture() = default;
		Texture(const Image& image, bool sRGB);
		Texture(const std::vector<std::shared_ptr<const Image>>& mips, bool sRGB);

		~Texture();

		static std::shared_ptr<Texture> depthTexture(const math::Vec2u& size);
		static std::shared_ptr<Texture> load(const std::string _name, bool sRGB = true, unsigned nChannels = 0);

		// Accessors
		auto glName() const { return mGLName; }

	private:

		static GLenum internalTexFormat(const Image& image, bool sRGB);
		static GLenum texFormat(const Image& image);

		GLuint mGLName = 0;
	};

}}
