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

#include <functional>
#include <string>
#include <vector>
#include <core/event.h>
#include <graphics/driver/shaderProcessor.h>
#include <core/platform/fileSystem/fileSystem.h>

namespace rev::gfx {

	class ShaderCodeFragment
	{
	public:
		// Default constructor
		ShaderCodeFragment() = default;
		// Constructor from code
		ShaderCodeFragment(const char* code);
		// Composition constructor
		ShaderCodeFragment(ShaderCodeFragment* a, ShaderCodeFragment* b);
		static ShaderCodeFragment* loadFromFile(const std::string& path);

		// Add all fragments of code
		void collapse(std::vector<std::string>& dst) const;

		using ReloadEvent = core::Event<ShaderCodeFragment&>;
		using ReloadListener = ReloadEvent::Listener;
		using ReloadCb = std::function<void(ShaderCodeFragment&)>;
		std::shared_ptr<ReloadListener> onReload(ReloadCb cb)
		{
			return m_onReload += cb;
		}

		const ShaderProcessor::MetaData& metadata() const
		{
			return m_metaData;
		}

	private:
		ShaderCodeFragment(const ShaderCodeFragment&) = delete;
		ShaderCodeFragment& operator=(const ShaderCodeFragment&) = delete;

		using FileListener = core::FileSystem::FileEvent::Listener;
		std::vector<std::shared_ptr<FileListener>> m_fileListeners;

		ReloadEvent m_onReload;

		ShaderProcessor::MetaData m_metaData;
		std::string m_srcCode;
		std::string m_processedCode;
		const ShaderCodeFragment* m_childA = nullptr;
		const ShaderCodeFragment* m_childB = nullptr;
	};

}	// namespace rev::gfx
