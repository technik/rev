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
#include <memory>
#include <graphics/driver/shader.h>
#include <graphics/scene/material.h>
#include <unordered_map>

namespace rev{ namespace graphics {

	class Camera;
	class GraphicsDriverGL;
	class RenderScene;
	class RenderTarget;

	class ForwardPass
	{
	public:
		ForwardPass(GraphicsDriverGL&);

		void render(const Camera&, const RenderScene&, const RenderTarget& _dst);

	private:
		void loadCommonShaderCode();
		Shader* loadShader(const std::string& fileName);
		void bindMaterial(const Material*);

		GraphicsDriverGL&	mDriver;
		float mEV;
		std::unique_ptr<Material>	mErrorMaterial;
		using ShaderPtr = std::unique_ptr<Shader>;

		std::string mForwardShaderCommonCode;
		std::unordered_map<std::string, ShaderPtr>	mPipelines;

		float mClearTimer = 0.f;
	};

}}
