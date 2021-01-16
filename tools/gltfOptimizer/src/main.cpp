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
#include <chrono>

using namespace fx; // gltf
using namespace rev::core;
using namespace std;
using namespace std::chrono;

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

constexpr float toMegabytes(size_t numBytes)
{
	return float(numBytes / 1024) / 1024;
}

struct SceneStats
{
public:
	size_t binFileSize = 0;
	size_t binDataSize = 0;
	size_t numBufferViews = 0;
	size_t numMeshes = 0;
	size_t numTriangles = 0;
	size_t numImages = 0;
	bool triangulated = true;
	SceneStats() = default;
	SceneStats(const string& scenePath, const gltf::Document& scene)
	{
		string binFileName = isBinaryFile(scenePath) ? scenePath : (scenePath.substr(0, scenePath.size() - 4) + "bin");
		binFileSize = std::filesystem::file_size(binFileName);
		numMeshes = scene.meshes.size();
		numBufferViews = scene.bufferViews.size();

		// Count triangles
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
		// Aggregate data size
		for (auto& buffer : scene.buffers)
		{
			binDataSize += buffer.byteLength;
		}
	}

	void dumpStatistics(std::ostream& out)
	{
		out << "Binary size:\t\t" << toMegabytes(binFileSize) << "mb" << endl
			<< "Num meshes:\t\t" << numMeshes << endl
			<< "Num images:\t\t" << numImages << endl
			<< "Num Buffer views:\t" << numBufferViews << endl
			<< "Data size:\t\t" << toMegabytes(binDataSize) << "mb" << endl;
		if (triangulated)
		{
			out << "Num triangles:\t\t" << numTriangles << endl;
			out << "Triangulated" << endl;
		}
		else
		{
			out << "Approx Num triangles:\t\t" << numTriangles << endl;
			out << "Not Triangulated" << endl;
		}
	}
};

void deduplicateBufferViews(gltf::Document& document)
{
	cout << "Removing redundant buffer views" << endl;
	std::vector<uint32_t> bvMapping(document.bufferViews.size(), 0);

	size_t numDuplicates = 0;
	size_t negativeTests = 0;
	size_t redundantMemory = 0;
	for (size_t i = 0; i < document.bufferViews.size(); ++i)
	{
		const auto& bv = document.bufferViews[i];
		// Check against all previous buffer views
		size_t j = 0;
		for (; j < i; ++j)
		{
			const auto& prevBv = document.bufferViews[j];
			if (prevBv.buffer == bv.buffer
				&& prevBv.byteLength == bv.byteLength
				&& prevBv.byteStride == bv.byteStride
				&& prevBv.target == bv.target)
			{
				const auto& buffer = document.buffers[bv.buffer];
				auto curData = buffer.data.data() + bv.byteOffset;
				auto prevData = buffer.data.data() + prevBv.byteOffset;
				if (memcmp(curData, prevData, bv.byteLength) == 0)
				{
					bvMapping[i] = (uint32_t)j;
					numDuplicates++;
					redundantMemory += bv.byteLength;
					break;
				}
				else negativeTests++;
			}
		}
		if (i == j) // No duplicate
			bvMapping[i] = i;
	}
	cout << "Found " << numDuplicates << " duplicate buffer views" << endl;
	cout << "Performed " << negativeTests << " negative memory compares of non-duplicates" << endl;
	cout << "Found " << redundantMemory << " redundant bytes of memory" << endl;
	cout << endl;
	// Replace identical buffer views
	for (auto& accessor : document.accessors)
	{
		auto bv = accessor.bufferView;
		if (bvMapping[bv])
		{
			accessor.bufferView = bvMapping[bv];
		}
	}

	std::vector<gltf::BufferView> usedBVs;
	for (size_t i = 0; i < bvMapping.size(); ++i)
	{
		if (bvMapping[i] == i) // Unique buffer view
		{
			bvMapping[i] = (uint32_t)usedBVs.size();
			usedBVs.push_back(document.bufferViews[i]);
		}
		else // Redundant, look up unique entry's mapping
		{
			bvMapping[i] = bvMapping[bvMapping[i]];
		}
	}
	document.bufferViews = usedBVs;

	// Substitute bufferViews
	for (auto& ac : document.accessors)
	{
		ac.bufferView = bvMapping[ac.bufferView];
	}

	// Clean up unused memory
	for (size_t i = 0; i < document.buffers.size(); ++i)
	{
		auto& buffer = document.buffers[i];
		std::vector<uint8_t> usedData;
		usedData.reserve(buffer.byteLength);

		for (size_t j = 0; j < document.bufferViews.size(); ++j)
		{
			auto& bv = document.bufferViews[j];
			if (bv.buffer != i) // This buffer view corresponts to a different buffer
				continue;

			auto offset = usedData.size();

			// Copy used data
			usedData.resize(usedData.size() + bv.byteLength);
			memcpy(usedData.data() + offset, buffer.data.data() + bv.byteOffset, bv.byteLength);

			bv.byteOffset = (uint32_t)offset;
		}

		buffer.byteLength = usedData.size();
		buffer.data = std::move(usedData);
	}
}

void deduplicateAccessors(gltf::Document& document)
{
	std::vector<uint32_t> acMapping(document.accessors.size(), 0);
	for (size_t i = 0; i < document.accessors.size(); ++i)
	{
		const auto& ac = document.accessors[i];

		// Find duplicate accessors
		size_t j = 0;
		for (; j < i; ++j)
		{
			const auto& prevAc = document.accessors[j];

			if (prevAc.bufferView == ac.bufferView
				&& prevAc.byteOffset == ac.byteOffset
				&& prevAc.componentType == ac.componentType
				&& prevAc.count == ac.count)
			{
				acMapping[i] = j;
				break;
			}
		}
		if (i == j) // No duplicate
			acMapping[i] = i;

		// Extract unique accessors
		std::vector<gltf::Accessor> usedAcs;
		for (size_t i = 0; i < acMapping.size(); ++i)
		{
			if (acMapping[i] == i) // Unique accessor
			{
				acMapping[i] = (uint32_t)usedAcs.size();
				usedAcs.push_back(document.accessors[i]);
			}
			else // Redundant, look up unique entry's mapping
			{
				acMapping[i] = acMapping[acMapping[i]];
			}
		}
		document.accessors = usedAcs;

		// Substitute bufferViews
		for (auto& mesh : document.meshes)
		{
			for (auto& primitive : mesh.primitives)
			{
				primitive.indices = acMapping[primitive.indices];
				// TODO: Normals, uvs, positions, ...
			}
		}
	}
}

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
	fx::gltf::ReadQuotas limits;
	limits.MaxFileSize = 1024 * 1024 * 1024; // 1 GB
	limits.MaxBufferByteLength = limits.MaxFileSize;

	cout << "Loading: " << options.inputScene << endl;

	high_resolution_clock timer;
	auto t0 = timer.now();
	fx::gltf::Document inputScene;
	try
	{
		bool binaryInput = isBinaryFile(options.inputScene);
		if (binaryInput)
			inputScene = fx::gltf::LoadFromBinary(options.inputScene, limits);
		else
			inputScene = fx::gltf::LoadFromText(options.inputScene, limits);
	}
	catch (fx::gltf::invalid_gltf_document e)
	{
		cout << "Json exception: " << e.what() << endl;
		return -1;
	}
	
	cout << "Load time: " << duration_cast<milliseconds>(timer.now() - t0).count() << "ms";

	// Display statistics
	cout << "------------------------------------------------" << endl
		<< "Initial stats" << endl;
	SceneStats initialStats(options.inputScene, inputScene);
	initialStats.dumpStatistics(cout);
	cout << endl;

	// Optimize scene
	deduplicateBufferViews(inputScene);
	deduplicateAccessors(inputScene);

	// Display final statistics
	cout << "------------------------------------------------" << endl
		<< "Final stats" << endl;
	SceneStats finalStats(options.inputScene, inputScene);
	finalStats.dumpStatistics(cout);
	cout << endl;

    return 0;
}