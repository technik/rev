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

#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>

#include <vector>

namespace rev :: gfx
{
	class Pipeline;
	class VertexArrayObject;
	class Texture;

	class CommandBuffer
	{
	public:
		// Command buffer lifetime
		virtual void begin() = 0;
		virtual void end() = 0;

		// Uniforms
		struct UniformBucket
		{
			// Management
			void clear();

			// Uniform binding
			void add(int pos, float f);
			void add(int pos, const math::Vec3f& f);
			void add(int pos, const math::Vec4f& f);
			void add(int pos, const math::Mat44f& m);
			void add(int pos, const std::vector<math::Mat44f>& matArray);
			void add(int pos, const Texture& tex);

			// Data access
			auto& floats()	const { return mFloatParams; }
			auto& vec3s()	const { return mVec3fParams; }
			auto& vec4s()	const { return mVec4fParams; }
			auto& mat4s()	const { return mMat44fParams; }
			auto& mat4vs()	const { return mMat44fArrayParams; }
			auto& textures()const { return mTextureParams; }

		private:
			std::pair<int,int>	mFloatParams;
			std::pair<int,int>	mVec3fParams;
			std::pair<int,int>	mVec4fParams;
			std::pair<int,int>	mMat44fParams;
			std::pair<int,int>	mMat44fArrayParams;
			std::pair<int,int>	mTextureParams;
		};

		// Commands
		virtual void setPipeline(const Pipeline&) = 0;
		virtual void setUniformData(const UniformBucket&) = 0;
		virtual void setVertexData(const VertexArrayObject&) = 0;
		virtual void drawTriangles(int numVertices) = 0;
		virtual void drawLines(int nVertices) = 0;

	};
}
