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
#include "Effect.h"
#include <sstream>
#include <graphics/driver/shader.h>
#include <graphics/driver/shaderProcessor.h>
#include <core/platform/fileSystem/fileSystem.h>

using namespace std;

namespace rev::gfx {

	//----------------------------------------------------------------------------------------------
	Effect::Effect(const string& _fileName)
		: m_fileName(_fileName)
	{
		ShaderProcessor::MetaData metadata;
		loadFromFile(_fileName.c_str(), metadata);
#ifdef _WIN32
		for(auto& dep : metadata.dependencies)
			core::FileSystem::get()->onFileChanged(dep) += [this](const char* fileName){ this->reload(); };
#endif
	}

	//----------------------------------------------------------------------------------------------
	const Effect::Property* Effect::property(const string& name) const
	{
		for(const auto& p : m_properties)
			if(p.name == name)
				return &p;

		return nullptr;
	}

	//----------------------------------------------------------------------------------------------
	void Effect::invokeCallbacks()
	{
		for(auto& cb : m_reloadCbs)
			cb(*this);
	}

	//----------------------------------------------------------------------------------------------
	void Effect::loadFromFile(const char* _fileName, ShaderProcessor::MetaData& metadata)
	{
		ShaderProcessor::loadCodeFromFile(_fileName, m_code, metadata);
		m_properties = metadata.uniforms;
		// TODO: Use pragmas and dependencies
	}

	//----------------------------------------------------------------------------------------------
	void Effect::reload()
	{
		m_properties.clear();
		m_code.clear();
		ShaderProcessor::MetaData metadata;
		loadFromFile(m_fileName.c_str(), metadata);
		invokeCallbacks();
	}
}