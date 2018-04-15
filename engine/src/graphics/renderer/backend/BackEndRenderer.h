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
#include <graphics/driver/openGL/openGL.h>
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
			Shader*	shader;

			std::pair<int,int>	mFloatParams;
			std::pair<int,int>	mVec3fParams;
			std::pair<int,int>	mVec4fParams;
			std::vector<std::pair<GLint,math::Mat44f>>	mMat44fParams;
			std::vector<std::pair<GLint,const Texture*>>	mTextureParams;

			void reset()
			{
				mMat44fParams.clear();
				mTextureParams.clear();
			}
		};

		BackEndRenderer(GraphicsDriverGL& driver)
			: mDriver(driver)
		{}

		void beginPass()
		{
			// Don't clear command list to prevent realocating resources inside each command
			mNumCommands = 0;
			for(auto& c : mCommandList)
				c.reset();
			mFloatIndices.clear();
			mFloatParams.clear();
			mVec3fIndices.clear();
			mVec3fParams.clear();
			mVec4fIndices.clear();
			mVec4fParams.clear();
		}

		void endCommand()
		{
			auto& cmd = mCommandList[mNumCommands++];
			cmd.mFloatParams.second = mFloatParams.size();
			cmd.mVec3fParams.second = mVec3fParams.size();
			cmd.mVec4fParams.second = mVec4fParams.size();
		}

		void endPass()
		{
		}

		void reserve(size_t minSize)
		{
			mCommandList.resize(std::max(mCommandList.size(), minSize));
		}

		Command& beginCommand()
		{
			// Finish previous command
			Command* cmd;
			if(mNumCommands < mCommandList.size()) // Reuse allocations
			{
				cmd = &mCommandList[mNumCommands];
			}
			else
			{
				mCommandList.emplace_back(Command());
				cmd = &mCommandList.back();
			}
			cmd->mFloatParams.first = mFloatParams.size();
			cmd->mVec3fParams.first = mVec3fParams.size();
			cmd->mVec4fParams.first = mVec4fParams.size();
			return *cmd;
		}

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

		void submitDraws()
		{
			GLuint vao = 0;
			Shader*	shader = nullptr;
			for(const auto& command : mCommandList)
			{
				if(shader != command.shader)
				{
					shader = command.shader;
					++usedShaders;
					shader->bind();
				}
				// Bind params
				for(int i = command.mFloatParams.first; i < command.mFloatParams.second; ++i)
				{
					mDriver.bindUniform(mFloatIndices[i], mFloatParams[i]);
				}
				for(int i = command.mVec3fParams.first; i < command.mVec3fParams.second; ++i)
				{
					mDriver.bindUniform(mVec3fIndices[i], mVec3fParams[i]);
				}
				for(int i = command.mVec4fParams.first; i < command.mVec4fParams.second; ++i)
				{
					mDriver.bindUniform(mVec4fIndices[i], mVec4fParams[i]);
				}
				for(const auto& m : command.mMat44fParams)
					mDriver.bindUniform(m.first, m.second);
				for(GLenum t = 0; t < command.mTextureParams.size(); ++t)
				{
					auto& textureParam = command.mTextureParams[t];
					glUniform1i(textureParam.first, t);
					glActiveTexture(GL_TEXTURE0+t);
					glBindTexture(GL_TEXTURE_2D, textureParam.second->glName());
				}
				// Bind geometry
				if(vao != command.vao)
				{
					vao = command.vao;
					glBindVertexArray(vao);
					++usedVaos;
				}
				// Draw
				glDrawElements(GL_TRIANGLES, command.nIndices, GL_UNSIGNED_SHORT, nullptr);
			}
		}

		void beginFrame()
		{
			usedVaos = 0;
			usedShaders = 0;
		}

		void drawStats()
		{
			ImGui::Text("Vaos: %d", usedVaos);
			ImGui::Text("Shaders: %d", usedShaders);
			ImGui::Text("CommandList size: %d", mCommandList.size());
		}

	private:
		// Stat counters
		// TODO: Share this between backends, or share the backend itself (worse for time coherency?)
		unsigned usedVaos;
		unsigned usedShaders;

		// Command lists
		std::vector<Command>	mCommandList;
		unsigned				mNumCommands;

		std::vector<float> mFloatParams;
		std::vector<GLint> mFloatIndices;
		std::vector<math::Vec3f> mVec3fParams;
		std::vector<GLint> mVec3fIndices;
		std::vector<math::Vec4f> mVec4fParams;
		std::vector<GLint> mVec4fIndices;

		GraphicsDriverGL&	mDriver;
	};

}}
