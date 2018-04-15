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
			GLuint vtxVbo;
			GLuint ndxVbo;
			unsigned nIndices;
			Shader*	shader;
			std::vector<std::pair<GLint,float>>			mFloatParams;
			std::vector<std::pair<GLint,math::Vec3f>>	mVec3fParams;
			std::vector<std::pair<GLint,math::Vec4f>>	mVec4fParams;
			std::vector<std::pair<GLint,math::Mat44f>>	mMat44fParams;
			std::vector<std::pair<GLint,Texture*>>		mTextureParams;

			void reset()
			{
				mFloatParams.clear();
				mVec3fParams.clear();
				mVec4fParams.clear();
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
		}

		void batchCommand(const Command& _drawCall)
		{
			if(mNumCommands < mCommandList.size()) // Reuse allocations
			{
				mCommandList[mNumCommands] = _drawCall;
			}
			else
			{
				mCommandList.push_back(_drawCall);
			}
			mNumCommands++;
		}

		void submitDraws()
		{
			GLuint vao = 0;
			Shader*	shader = nullptr;
			for(const auto& command : mCommandList)
			{
				if(vao != command.vao)
				{
					glBindVertexArray(vao);
					vao = command.vao;
					++usedVaos;
				}
				if(shader != command.shader)
				{
					shader = command.shader;
					++usedShaders;
					shader->bind();
				}
				// Bind params
				for(const auto& f : command.mFloatParams)
					mDriver.bindUniform(f.first, f.second);
				for(const auto& v : command.mVec3fParams)
					mDriver.bindUniform(v.first, v.second);
				for(const auto& v : command.mVec4fParams)
					mDriver.bindUniform(v.first, v.second);
				for(const auto& m : command.mMat44fParams)
					mDriver.bindUniform(m.first, m.second);
				for(GLenum t = 0; t < command.mTextureParams.size(); ++t)
				{
					auto& textureParam = command.mTextureParams[t];
					glUniform1i(textureParam.first, t);
					glActiveTexture(GL_TEXTURE0+t);
					glBindTexture(GL_TEXTURE_2D, textureParam.second->glName());
				}
				glDrawElements(GL_TRIANGLES, command.nIndices, GL_UNSIGNED_SHORT, nullptr);
			}
		}

		static void beginFrame()
		{
			usedVaos = 0;
			usedShaders = 0;
		}

		static void drawStats()
		{
			ImGui::Text("Vaos: %d", usedVaos);
			ImGui::Text("Draw Calls: %d", usedShaders);
		}

	private:
		// Stat counters
		static unsigned usedVaos;
		static unsigned usedShaders;

		// Command lists
		std::vector<Command>	mCommandList;
		unsigned				mNumCommands;

		GraphicsDriverGL&	mDriver;
	};

}}
