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
#include "project.h"
#include <fstream>
#include <graphics/debug/imgui.h>

using rev::core::Json;
using namespace std;

namespace rev { namespace editor {

	//----------------------------------------------------------------------------------------------
	Project::Project()
	{
		registerInspectorFactories();
	}

	//----------------------------------------------------------------------------------------------
	void Project::registerInspectorFactories()
	{
		//registerFactory<MaterialInspector>(string("material"));
	}

	//----------------------------------------------------------------------------------------------
	bool Project::load(const string& _fileName)
	{
		ifstream in(_fileName);
		if(!in)
			return false;
		Json projectData;
		in >> projectData;
		mRootFolder.load(projectData["rootFolder"], mMaterialList);
		mFileName = _fileName;
		return true;
	}

	//----------------------------------------------------------------------------------------------
	bool Project::save() const {
		ofstream out(mFileName);
		if(!out)
			return false;
		Json serializedProject;
		auto& projectFolders = serializedProject["rootFolder"];
		projectFolders = Json({});
		if(!mRootFolder.save(serializedProject["rootFolder"]))
			return false;
		out << serializedProject;
		return true;
	}

	//----------------------------------------------------------------------------------------------
	bool Project::saveAs(const string& _fileName)
	{
		mFileName = _fileName;
		return save();
	}

	//----------------------------------------------------------------------------------------------
	void Project::showExplorer() {
		ImGui::Begin("Project Explorer", nullptr, ImGuiWindowFlags_::ImGuiWindowFlags_NoCollapse);
		// Project tree
		ImGui::BeginGroup();
		ImGui::BeginChild("Folder tree", ImVec2(200,0), true);
			ImGui::Text("Folder Tree");
			ImGui::Separator();
			for(auto& f : mRootFolder.mChildren)
			{
				if(ImGui::Selectable(f.name.c_str(), mSelectedFolder == &f))
					mSelectedFolder = &f;
			}
		ImGui::EndChild();
		ImGui::EndGroup();

		// Folder view
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::BeginChild("Folder View", ImVec2(400,0), true);
			ImGui::Text("Folder View");
			ImGui::Separator();
			if(mSelectedFolder)
			{
				auto oldAsset = mSelectedAsset;
				for(auto& asset : mSelectedFolder->mAssets)
				{
					if(ImGui::Selectable(asset.name.c_str(), mSelectedAsset == &asset))
					{
						if(mSelectedAsset != &asset)
						{
							mSelectedAsset = &asset;
							auto factoryIterator = mInspectorFactories.find(asset.type);
							if(factoryIterator == mInspectorFactories.end())
							{
								mSelectedAsset = nullptr;
								mSelectedInspector = nullptr;
							}
							else {
								mSelectedInspector = factoryIterator->second(asset);
								mSelectedAsset = &asset;
							}
						}
					}
				}
			}
		ImGui::EndChild();
		ImGui::EndGroup();

		// Asset inspector
		ImGui::SameLine();
		ImGui::BeginGroup();
		ImGui::BeginChild("Asset Inspector", ImVec2(200,0), true);
			ImGui::Text("Asset Inspector");
			ImGui::Separator();
			//if(mSelectedInspector)
			//	mSelectedInspector->showInspectionPanel();
		ImGui::EndChild();
		ImGui::EndGroup();

		ImGui::End();
	}

	//----------------------------------------------------------------------------------------------
	void Project::Folder::load(
		const Json& data,
		vector<string>&	materialList)
	{
		// Load nested folders
		auto nestedFolders = data.find("folders");
		if(nestedFolders != data.end())
		{
			auto& children = nestedFolders.value();
			auto n = children.size();
			mChildren.reserve(n);
			for(auto i = children.begin(); i != children.end(); ++i)
			{
				const auto& folderName = i.key();
				auto folderPath = path + folderName + "/";
				mChildren.push_back({ folderName, folderPath });
				mChildren.back().load(i.value(), materialList);
			}
		}
		// Load assets
		auto folderAssets = data.find("assets");
		if(folderAssets != data.end())
		{
			auto& assets = folderAssets.value();
			auto n = assets.size();
			mAssets.reserve(n);
			for(auto& assetData : assets)
			{
				string assetName = assetData["name"];
				string assetType = assetData["type"];
				if(assetType == "material")
				{
					materialList.push_back(path+assetName);
				}
				mAssets.push_back({assetType, assetName, path});
			}
		}
	}

	//----------------------------------------------------------------------------------------------
	bool Project::Folder::save(Json& dst) const
	{
		if(!mChildren.empty())
		{
			auto childFolders = dst["folders"];
			childFolders = Json({});
			auto n = mChildren.size();
			for(auto& folder : mChildren)
			{
				if(!folder.save(childFolders[folder.name]))
					return false;
			}
		}
		return true;
	}

}}