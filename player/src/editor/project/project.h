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
#include <unordered_map>
#include <string>
#include <core/types/json.h>
#include "asset.h"

namespace rev { namespace editor {

	class AssetInspector;

	class Project
	{
	public:
		Project();

		bool load(const std::string& _fileName);
		bool save() const;
		bool saveAs(const std::string& _fileName);

		void showExplorer();

		const std::vector<std::string> materials() const { return mMaterialList; }

	private:
		void registerInspectorFactories();

		struct Folder
		{
			void load(const core::Json&, std::vector<std::string>&	materialList);
			bool save(core::Json&) const;

			const std::string name;
			const std::string path;

			std::vector<Folder>	mChildren;
			std::vector<Asset>	mAssets;
		};

		template<class T>
		void registerFactory(const std::string& _assetType)
		{
			mInspectorFactories.insert(make_pair(
				_assetType, [](Asset& asset) {
				return std::make_shared<T>(asset);
			}));
		}

		using InspectorFactory = std::function<std::shared_ptr<AssetInspector>(Asset&)>;

		std::string mFileName;
		Folder mRootFolder;
		Folder* mSelectedFolder = nullptr;
		Asset* mSelectedAsset = nullptr;
		std::shared_ptr<AssetInspector>						mSelectedInspector;
		std::unordered_map<std::string,InspectorFactory>	mInspectorFactories;

		std::vector<std::string>	mMaterialList;
	};

}}	// rev::editor