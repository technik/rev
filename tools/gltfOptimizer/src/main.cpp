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
	size_t numEmissiveTriangles = 0;
	size_t numImages = 0;
	size_t numMeshInstances = 0;
	size_t numEmissivePrimitives = 0;
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
				if (primitive.material >= 0)
				{
					auto& mat = scene.materials[primitive.material];
					if (mat.emissiveFactor[0] > 0 || mat.emissiveFactor[1] > 0 || mat.emissiveFactor[2] > 0)
						numEmissiveTriangles += primitiveNumIndices;
				}
				if (primitive.mode != gltf::Primitive::Mode::Triangles)
				{
					triangulated = false;
				}
			}
		}
		numTriangles /= 3; // Exact for tris, approx for other
		numEmissiveTriangles /= 3;
		// Aggregate data size
		for (auto& buffer : scene.buffers)
		{
			binDataSize += buffer.byteLength;
		}
		// Count mesh instances
		for (auto& node : scene.nodes)
		{
			if (node.mesh >= 0)
			{
				++numMeshInstances;
				auto& mesh = scene.meshes[node.mesh];
				for (auto& p : mesh.primitives)
				{
					if (p.material >= 0)
					{
						auto& mat = scene.materials[p.material];
						if (mat.emissiveFactor[0] > 0 || mat.emissiveFactor[1] > 0 || mat.emissiveFactor[2] > 0)
							numEmissivePrimitives++;
					}
				}
			}
		}
		// Count images
		for (auto& images : scene.images)
			++numImages;
	}

	void dumpStatistics(std::ostream& out)
	{
		out << "Binary size:\t\t" << toMegabytes(binFileSize) << "mb" << endl
			<< "Num meshes:\t\t" << numMeshes << endl
			<< "Num mesh instances:\t" << numMeshInstances << endl
			<< "Num emissive primitive instances:\t" << numEmissivePrimitives << endl
			<< "Num images:\t\t" << numImages << endl
			<< "Num Buffer views:\t" << numBufferViews << endl
			<< "Data size:\t\t" << toMegabytes(binDataSize) << "mb" << endl;
		if (triangulated)
		{
			out << "Num triangles:\t\t" << numTriangles << endl;
			out << "Num Emissive triangles:\t" << numEmissiveTriangles << endl;
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
		if (i == j) // Not a duplicate
			bvMapping[i] = i;
	}
	cout << "Found " << numDuplicates << " duplicate buffer views" << endl;
	cout << "Performed " << negativeTests << " negative memory compares of non-duplicates" << endl;
	cout << "Found " << redundantMemory << " redundant bytes of memory" << endl;
	cout << endl;

	// Eliminate unused buffer views
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

	// Replace identical buffer views
	for (auto& accessor : document.accessors)
	{
		auto bv = accessor.bufferView;
		if (bv != uint32_t(-1))
		{
			accessor.bufferView = bvMapping[bv];
		}
	}
	for (auto& image : document.images)
	{
		if (image.bufferView != uint32_t(-1))
		{
			image.bufferView = bvMapping[image.bufferView];
		}
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
		if (ac.bufferView == -1)
		{
			acMapping[i] = i;
			break;
		}

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
		if (i == j) // Not a duplicate
			acMapping[i] = i;
	}

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
			for (auto& attr : primitive.attributes)
			{
				attr.second = acMapping[attr.second];
			}
		}
	}
	// Animation samplers
	/*for (auto& anim : document.animations)
	{
		for (auto& sampler : anim.samplers)
		{
			sampler.input = acMapping[sampler.input];
			sampler.output = acMapping[sampler.output];
		}
	}*/
	// TODO: Skin inverse bind matrices
}

bool operator==(const gltf::Primitive& a, const gltf::Primitive& b)
{
	if (a.indices != b.indices)
		return false;
	if (a.material != b.material)
		return false;
	if (a.mode != b.mode)
		return false;
	if (a.attributes.size() != b.attributes.size())
		return false;
	for (auto& attr : a.attributes)
	{
		auto bAttr = b.attributes.find(attr.first);
		if (bAttr == b.attributes.end())
		{
			if (bAttr->second != attr.second)
				return false;
		}
	}
	return true;
}

bool operator==(const gltf::Mesh& a, const gltf::Mesh& b)
{
	if (a.primitives.size() != b.primitives.size())
		return false;
	if (a.weights.size() != b.weights.size())
		return false;
	if (a.weights.size() > 0)
	{
		return memcmp(a.weights.data(), b.weights.data(), a.weights.size()) != 0;
	}
	for (size_t i = 0; i < a.primitives.size(); ++i)
	{
		bool eqPrim = a.primitives[i] == b.primitives[i];
		if (!eqPrim)
			return false;
	}
	return true;
}

void deduplicateMeshes(gltf::Document& document)
{
	// Find duplicate meshes
	std::vector<uint32_t> meshMapping(document.meshes.size(), 0);
	for (size_t i = 0; i < document.meshes.size(); ++i)
	{
		const auto& mesh = document.meshes[i];

		// Compare against previous meshes
		size_t j = 0;
		for (; j < i; ++j)
		{
			const auto& prevMesh = document.meshes[j];
			if(prevMesh == mesh)
			{
				meshMapping[i] = j;
				break;
			}
		}
		if (i == j) // Not a duplicate
			meshMapping[i] = i;
	}

	// Extract unique meshes
	std::vector<gltf::Mesh> uniqueMeshes;
	for (size_t i = 0; i < meshMapping.size(); ++i)
	{
		if (meshMapping[i] == i) // Unique mesh
		{
			meshMapping[i] = (uint32_t)uniqueMeshes.size();
			uniqueMeshes.push_back(document.meshes[i]);
		}
		else // Redundant, look up unique entry's mapping
		{
			meshMapping[i] = meshMapping[meshMapping[i]];
		}
	}
	document.meshes = uniqueMeshes;

	// Substitute mesh indices
	for (auto& node : document.nodes)
	{
		if (node.mesh >= 0)
		{
			node.mesh = meshMapping[node.mesh];
		}
	}
}

bool operator==(const gltf::Image& a, const gltf::Image& b)
{
	if (a.bufferView > -1 && b.bufferView == a.bufferView)
	{
		return true;
	}

	if (b.bufferView > -1)
		return false;

	return a.uri == b.uri;
}

void deduplicateImages(gltf::Document& document)
{
	// Find duplicate images
	std::vector<uint32_t> imageMapping(document.images.size());
	for (size_t i = 0; i < document.images.size(); ++i)
	{
		const auto& image = document.images[i];

		// Compare against previous images
		size_t j = 0;
		for (; j < i; ++j)
		{
			const auto& prevImage = document.images[j];
			if(prevImage == image)
			{
				imageMapping[i] = j;
				break;
			}
		}
		if (i == j) // Not a duplicate
			imageMapping[i] = i;
	}

	// Extract unique images
	std::vector<gltf::Image> uniqueImages;
	for (size_t i = 0; i < imageMapping.size(); ++i)
	{
		if (imageMapping[i] == i) // Unique image
		{
			imageMapping[i] = (uint32_t)uniqueImages.size();
			uniqueImages.push_back(document.images[i]);
		}
		else // Redundant image. Look up unique entry's final mapping
		{
			imageMapping[i] = imageMapping[imageMapping[i]];
		}
	}
	document.images = uniqueImages;

	// Substitute image indices
	for (auto& texture : document.textures)
	{
		if (texture.source > -1)
		{
			texture.source = imageMapping[texture.source];
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
		auto inFile = filesystem::path(options.inputScene);
		auto inFolder = inFile.parent_path();
		auto outFolder = inFolder / "out";
		options.outputScene = (outFolder / inFile.filename()).string();
	}

	// Load input scene
	fx::gltf::ReadQuotas limits;
	limits.MaxFileSize = 1024 * 1024 * 1024; // 1 GB
	limits.MaxBufferByteLength = limits.MaxFileSize;

	cout << "Loading: " << options.inputScene << endl;

	high_resolution_clock timer;
	auto t0 = timer.now();
	fx::gltf::Document scene;
	try
	{
		bool binaryInput = isBinaryFile(options.inputScene);
		if (binaryInput)
			scene = fx::gltf::LoadFromBinary(options.inputScene, limits);
		else
			scene = fx::gltf::LoadFromText(options.inputScene, limits);
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
	SceneStats initialStats(options.inputScene, scene);
	initialStats.dumpStatistics(cout);
	cout << endl;

	// Optimize scene
	deduplicateBufferViews(scene);
	deduplicateAccessors(scene);
	deduplicateMeshes(scene);
	deduplicateImages(scene);

	// Save optimized model
	try {
		filesystem::path outPath(options.outputScene);
		auto outFolder = outPath.parent_path();
		if (!filesystem::exists(outFolder))
			filesystem::create_directory(outFolder);
		gltf::Save(scene, options.outputScene, isBinaryFile(options.outputScene));
	}
	catch(runtime_error e)
	{
		cout << "Exception thrown trying to save optimized model" << endl;
		cout << e.what() << endl;
		return -2;
	}
	catch (exception e)
	{
		cout << "Exception thrown trying to save optimized model" << endl;
		cout << e.what() << endl;
		return -3;
	}

	// Display final statistics
	cout << "------------------------------------------------" << endl
		<< "Final stats" << endl;
	SceneStats finalStats(options.outputScene, scene);
	finalStats.dumpStatistics(cout);
	cout << endl;

    return 0;
}