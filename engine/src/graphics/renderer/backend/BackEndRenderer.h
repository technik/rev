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

#include <utility>
#include <vector>
#include <graphics/debug/imgui.h>
#include <graphics/backend/openGL/openGL.h>
#include <graphics/driver/shader.h>
#include <graphics/driver/texture.h>
#include <math/algebra/vector.h>

namespace rev{ namespace graphics {

	class BackEndRenderer
	{
	public:
		struct Command
		{
			GLuint vao;
			unsigned nIndices;
			const Shader*	shader;
			GLenum cullMode;
			GLenum indexType;

			std::pair<int,int>	mFloatParams;
			std::pair<int,int>	mVec3fParams;
			std::pair<int,int>	mVec4fParams;
			std::pair<int,int>	mMat44fParams;
			std::pair<int,int>	mMat44fArrayParams;
			std::pair<int,int>	mTextureParams;
		};

		BackEndRenderer(GraphicsDriverGL& driver)
			: mDriver(driver)
		{}

		// Frame
		void beginFrame();
		void drawStats();

		// Pass
		void beginPass();
		void endPass();
		void submitDraws();

		// Command
		void reserve(size_t minSize);
		Command& beginCommand();
		void endCommand();

		// Params
		void addParam(GLint index, float f)
		{
			mFloatIndices.push_back(index);
			mFloatParams.push_back(f);
		}

		void addParam(GLint index, const math::Vec3f& v)
		{
			mVec3fIndices.push_back(index);
			mVec3fParams.push_back(v);
		}

		void addParam(GLint index, const math::Vec4f& v)
		{
			mVec4fIndices.push_back(index);
			mVec4fParams.push_back(v);
		}

		void addParam(GLint index, const math::Mat44f& v)
		{
			mMat44fIndices.push_back(index);
			mMat44fParams.push_back(v);
		}

		void addParam(GLint index, const std::vector<math::Mat44f>& v)
		{
			mMat44fArrayIndices.push_back(index);
			mMat44fArrayParams.push_back(v);
		}

		void addParam(GLint index, const Texture* v)
		{
			mTextureIndices.push_back(index);
			mTextureParams.push_back(v);
		}

	private:
		// Stat counters
		// TODO: Share this between backends, or share the backend itself (worse for time coherency?)
		unsigned usedVaos;
		unsigned usedShaders;
		unsigned triangles;

		// Command lists
		std::vector<Command>	mCommandList;
		unsigned				mNumCommands;

		std::vector<float> mFloatParams;
		std::vector<GLint> mFloatIndices;
		std::vector<math::Vec3f> mVec3fParams;
		std::vector<GLint> mVec3fIndices;
		std::vector<math::Vec4f> mVec4fParams;
		std::vector<GLint> mVec4fIndices;
		std::vector<math::Mat44f>	mMat44fParams;
		std::vector<GLint> mMat44fArrayIndices;
		std::vector<std::vector<math::Mat44f>> mMat44fArrayParams;
		std::vector<GLint> mMat44fIndices;
		std::vector<const Texture*>	mTextureParams;
		std::vector<GLint> mTextureIndices;

		GraphicsDriverGL&	mDriver;
	};

}}
