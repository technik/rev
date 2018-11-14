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
#include "shaderCodeFragment.h"
#include <core/platform/fileSystem/fileSystem.h>

namespace rev::gfx {

	//----------------------------------------------------------------------------------------------
	ShaderCodeFragment::ShaderCodeFragment(const char* code)
		: m_srcCode(code)
	{
		ShaderProcessor::Context context;
		context.m_pendingCode.push_back({0, m_srcCode});
		ShaderProcessor::processCode(context, true, m_processedCode, m_metaData);

		// Setup reload callback for all dependencies
		for(auto& file : m_metaData.dependencies)
		{
			core::FileSystem::get()->onFileChanged(file) += [this](const char*) {
				ShaderProcessor::Context context;
				context.m_pendingCode.push_back({0, m_srcCode});
				ShaderProcessor::processCode(context, true, m_processedCode, m_metaData);
			};
		}
	}

	//----------------------------------------------------------------------------------------------
	ShaderCodeFragment::ShaderCodeFragment(ShaderCodeFragment& a, ShaderCodeFragment& b)
		: m_childA(&a)
		, m_childB(&b)
	{
		// reload when either child reloads
		auto reloadCb = [this](const ShaderCodeFragment&)
		{
			this->invokeReload();
		};
		a.onReload(reloadCb);
		b.onReload(reloadCb);
	}

	//----------------------------------------------------------------------------------------------
	ShaderCodeFragment ShaderCodeFragment::loadFromFile(const std::string& path)
	{
		ShaderCodeFragment fragment;
		ShaderProcessor::loadCodeFromFile(path, fragment.m_processedCode, fragment.m_metaData);

		// Subscribte to file changes and dependencies
		for(auto& file : fragment.m_metaData.dependencies)
		{
			core::FileSystem::get()->onFileChanged(file) += [&fragment, path](const char*) {
				ShaderProcessor::loadCodeFromFile(path, fragment.m_processedCode, fragment.m_metaData);
			};
		}
		return fragment;
	}

	//----------------------------------------------------------------------------------------------
	void ShaderCodeFragment::collapse(std::vector<std::string>& out) const
	{
		if(!m_processedCode.empty())
		{
			out.push_back(m_processedCode);
		}
		if(m_childA)
			m_childA->collapse(out);
		if(m_childB)
			m_childB->collapse(out);
	}

}