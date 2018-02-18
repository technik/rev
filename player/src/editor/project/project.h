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
#include <core/types/json.h>
#include <string>

namespace rev { namespace editor {

	class AssetInspector
	{
	public:
		virtual void inspect(const std::string& path) = 0;
	};

	class Project
	{
	public:
		bool load(const std::string& _fileName);
		bool save() const;
		bool saveAs(const std::string& _fileName);

		void showExplorer();

	private:
		struct Asset
		{
			std::string type;
			std::string name;
			std::string path;
		};

		struct Folder
		{
			void load(const core::Json&);
			bool save(core::Json&) const;

			const std::string name;
			const std::string path;

			std::vector<Folder>	mChildren;
			std::vector<Asset>	mAssets;
		};

		std::string mFileName;
		Folder mRootFolder;
		Folder* mSelectedFolder = nullptr;
	};

}}	// rev::editor