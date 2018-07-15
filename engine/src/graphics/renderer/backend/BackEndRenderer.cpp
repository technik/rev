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

#include "BackEndRenderer.h"

namespace rev::graphics {

	//------------------------------------------------------------------------------------------------------------------
	void BackEndRenderer::beginPass()
	{
		// Don't clear command list to prevent realocating resources inside each command
		mNumCommands = 0;
		// Clear uniform indices
		mFloatIndices.clear();
		mVec3fIndices.clear();
		mVec4fIndices.clear();
		mMat44fIndices.clear();
		mMat44fArrayIndices.clear();
		mTextureIndices.clear();
		// Clear uniform params
		mFloatParams.clear();
		mVec3fParams.clear();
		mVec4fParams.clear();
		mMat44fParams.clear();
		mMat44fArrayParams.clear();
		mTextureParams.clear();
	}

	//------------------------------------------------------------------------------------------------------------------
	void BackEndRenderer::endCommand()
	{
		auto& cmd = mCommandList[mNumCommands++];
		cmd.mFloatParams.second = mFloatParams.size();
		cmd.mVec3fParams.second = mVec3fParams.size();
		cmd.mVec4fParams.second = mVec4fParams.size();
		cmd.mMat44fParams.second = mMat44fParams.size();
		cmd.mMat44fArrayParams.second = mMat44fArrayParams.size();
		cmd.mTextureParams.second = mTextureParams.size();
	}

	//------------------------------------------------------------------------------------------------------------------
	void BackEndRenderer::endPass()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void BackEndRenderer::reserve(size_t minSize)
	{
		mCommandList.resize(std::max(mCommandList.size(), minSize));
	}

	//------------------------------------------------------------------------------------------------------------------
	BackEndRenderer::Command& BackEndRenderer::beginCommand()
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
		cmd->mMat44fParams.first = mMat44fParams.size();
		cmd->mMat44fArrayParams.first = mMat44fArrayParams.size();
		cmd->mTextureParams.first = mTextureParams.size();
		return *cmd;
	}

	void BackEndRenderer::submitDraws()
	{
		GLuint vao = 0;
		const Shader*	shader = nullptr;
		for(size_t k = 0; k < mNumCommands; ++k)
		{
			const auto& command = mCommandList[k];
			if(shader != command.shader)
			{
				shader = command.shader;
				++usedShaders;
				shader->bind();
			}
			glCullFace(command.cullMode);
			// Bind params
			for(int i = command.mFloatParams.first; i < command.mFloatParams.second; ++i)
				mDriver.bindUniform(mFloatIndices[i], mFloatParams[i]);
			for(int i = command.mVec3fParams.first; i < command.mVec3fParams.second; ++i)
				mDriver.bindUniform(mVec3fIndices[i], mVec3fParams[i]);
			for(int i = command.mVec4fParams.first; i < command.mVec4fParams.second; ++i)
				mDriver.bindUniform(mVec4fIndices[i], mVec4fParams[i]);
			for(int i = command.mMat44fParams.first; i < command.mMat44fParams.second; ++i)
				mDriver.bindUniform(mMat44fIndices[i], mMat44fParams[i]);
			for(int i = command.mMat44fArrayParams.first; i < command.mMat44fArrayParams.second; ++i)
				mDriver.bindUniform(mMat44fArrayIndices[i], mMat44fArrayParams[i]);
			//int textureStage = 0;
			for(int i = command.mTextureParams.first; i < command.mTextureParams.second; ++i)
			{
				auto index = mTextureIndices[i];
				glActiveTexture(GL_TEXTURE0+index);
				glBindTexture(GL_TEXTURE_2D, mTextureParams[i]->glName());
				glUniform1i(index, index);
			}
			// Bind geometry
			if(vao != command.vao)
			{
				vao = command.vao;
				glBindVertexArray(vao);
				++usedVaos;
			}
			// Draw
			glDrawElements(GL_TRIANGLES, command.nIndices, command.indexType, nullptr);
			triangles += command.nIndices/3;
		}
		mNumCommands = 0; // Reset command list
	}

	void BackEndRenderer::beginFrame()
	{
		usedVaos = 0;
		usedShaders = 0;
		triangles = 0;
	}

	void BackEndRenderer::drawStats()
	{
		ImGui::Text("Vaos: %d", usedVaos);
		ImGui::Text("Shaders: %d", usedShaders);
		ImGui::Text("Triangles: %d", triangles);
		ImGui::Text("float params: %d", mFloatParams.size());
		ImGui::Text("Vec3f params: %d", mVec3fParams.size());
		ImGui::Text("Texture params: %d", mTextureParams.size());
	}
}