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
	/*ShaderCodeFragment::ShaderCodeFragment(const char* code)
		: m_srcCode(code)
	{
		ShaderProcessor::Context context;
		context.m_pendingCode.push_back({0, m_srcCode});
		ShaderProcessor::processCode(context, true, m_processedCode, m_metaData);

		// Setup reload callback for all dependencies
		auto onFileChanged = [this](const char*) {
			ShaderProcessor::Context context;
			context.m_pendingCode.push_back({0, m_srcCode});
			this->m_processedCode.clear();
			this->m_metaData.clear();
			ShaderProcessor::processCode(context, true, m_processedCode, m_metaData);
			this->m_onReload(*this);
		};
		for(auto& file : m_metaData.dependencies)
		{
			m_fileListeners.push_back(core::FileSystem::get()->onFileChanged(file) += onFileChanged);
		}
	}

	//----------------------------------------------------------------------------------------------
	ShaderCodeFragment::ShaderCodeFragment(ShaderCodeFragment* a, ShaderCodeFragment* b)
		: m_childA(a)
		, m_childB(b)
	{
		// reload when either child reloads
		auto reloadCb = [this](const ShaderCodeFragment&)
		{
			this->m_onReload(*this);
		};
		a->onReload(reloadCb);
		b->onReload(reloadCb);
	}

	//----------------------------------------------------------------------------------------------
	ShaderCodeFragment* ShaderCodeFragment::loadFromFile(const std::string& path)
	{
		auto fragment = new ShaderCodeFragment();
		ShaderProcessor::loadCodeFromFile(path, fragment->m_processedCode, fragment->m_metaData);

		// Subscribte to file changes and dependencies
		auto onFileChanged = [fragment, path](const char*) {
			fragment->m_processedCode.clear();
			fragment->m_metaData.clear();
			ShaderProcessor::loadCodeFromFile(path, fragment->m_processedCode, fragment->m_metaData);
			fragment->m_onReload(*fragment);
		};
		for(auto& file : fragment->m_metaData.dependencies)
		{
			fragment->m_fileListeners.push_back(core::FileSystem::get()->onFileChanged(file) += onFileChanged);
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
	}*/
}