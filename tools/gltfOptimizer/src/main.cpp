//--------------------------------------------------------------------------------------------------
// glTF Optimizer
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
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
#include <gltf.h>
#include <core/platform/cmdLineParser.h>
#include <filesystem>

#include <string>
#include <unordered_set>
#include <iostream>

using namespace fx; // gltf
using namespace rev::core;
using namespace std;

struct Options
{
	string inputScene;
	string outputScene;

	void parseCommandLine(int argc, const char** argv)
	{
		CmdLineParser cmd;
		cmd.addOption("input", &inputScene);
		cmd.addOption("output", &inputScene);

		cmd.parse(argc, argv);
	}
};

bool isBinaryFile(const string& path)
{
	return path.substr(path.size() - 4) == ".glb";
}

struct SceneStats
{
public:
	size_t binFileSize = 0;
	size_t numMeshes = 0;
	size_t numTriangles = 0;
	size_t numUniqueTriangles = 0;
	size_t numImages = 0;
	bool triangulated = true;
	SceneStats() = default;
	SceneStats(const string& scenePath, const gltf::Document& scene)
	{
		string binFileName = isBinaryFile(scenePath) ? scenePath : (scenePath.substr(scenePath.size() - 4) + ".bin");
		binFileSize = std::filesystem::file_size(binFileName);
		numMeshes = scene.meshes.size();
		unordered_set<uint32_t> indicesAccessors;
		for (const auto& mesh : scene.meshes)
		{
			for (const auto& primitive : mesh.primitives)
			{
				auto indicesAccess = primitive.indices;
				indicesAccessors.insert(indicesAccess);
				auto& accessor = scene.accessors[indicesAccess];
				auto primitiveNumIndices = accessor.count;
				numTriangles += primitiveNumIndices;
				if (primitive.mode != gltf::Primitive::Mode::Triangles)
				{
					triangulated = false;
				}
			}
		}
		numTriangles /= 3; // Exact for tris, approx for other
	}

	void dumpStatistics(std::ostream& out)
	{
		out << "Binary size:\t\t" << binFileSize << endl
			<< "Num meshes:\t\t " << numTriangles << endl
			<< "Num images:\t " << numImages << endl;
		if (triangulated)
		{
			out << "Num triangles:\t\t " << numTriangles << endl
				<< "Num unique triangles:\t " << numUniqueTriangles << endl;
		}
		else
		{
			out << "Approx Num triangles:\t\t " << numTriangles << endl
				<< "Approx Num unique triangles:\t " << numUniqueTriangles << endl;
		}
	}
};


int main(int argc, const char** argv)
{
	// Process command line options
	Options options;
	options.parseCommandLine(argc, argv);

	if (options.inputScene.empty())
	{
		cout << "No input scene provided\n";
		return -1;
	}
	if (options.outputScene.empty())
	{
		options.outputScene = options.inputScene.substr(0, options.inputScene.size() - 5) + "_out.gltf";
	}

	// Load input scene
	fx::gltf::Document inputScene;
	bool binaryInput = isBinaryFile(options.inputScene);
	if (binaryInput)
		inputScene = fx::gltf::LoadFromBinary(options.inputScene);
	else
		inputScene = fx::gltf::LoadFromText(options.inputScene);

	// Display statistics
	dumpStatistics(inputScene, cout);

    return 0;
}