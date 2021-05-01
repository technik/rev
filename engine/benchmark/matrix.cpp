//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
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
#include <benchmark/benchmark.h>
#include <math/algebra/matrix.h>
#include <vector>
#include <random>

using namespace rev::math;

std::default_random_engine rng;
std::uniform_real_distribution reals(0.f, 100.f);

static void ContiguousMatrixTree(benchmark::State& state)
{
	// -- Set up ----
	// Allocate matrices
	const size_t numMatrices = state.range();
	std::vector<Mat44f> matrices(numMatrices);
	// Randomize data
	for (size_t i = 0; i < numMatrices; ++i)
		for (size_t j = 0; j < 16; ++j)
			matrices[i].data()[j] = reals(rng);

	// Allocate parent indices
	std::vector<size_t> parentIndices(numMatrices);
	// Randomize parent indices
	parentIndices[0] = 0;
	std::uniform_int_distribution<size_t> ints(0, numMatrices);
	for (size_t i = 1; i < numMatrices; ++i)
		parentIndices[i] = ints(rng) % i;

	// --- Run the actual benchmark ---
	while (state.KeepRunning())
	{
		for (size_t i = 1; i < numMatrices; ++i)
		{
			auto parent = parentIndices[i];
			matrices[i] = matrices[parent] * matrices[i];
		}
	}
}

static void InterleavedMatricesTree(benchmark::State& state)
{
	// -- Set up ----
	const size_t numMatrices = state.range();
	using Node = std::pair<Mat44f, size_t>;
	// Allocate matrices
	std::vector<Node> tree(numMatrices);
	// Randomize data
	std::uniform_int_distribution<size_t> ints(0, numMatrices);
	for (size_t i = 0; i < numMatrices; ++i)
	{
		for (size_t j = 0; j < 16; ++j)
			tree[i].first.data()[j] = reals(rng);
		tree[i].second = i ? ints(rng) % i : 0;
	}

	// --- Run the actual benchmark ---
	while (state.KeepRunning())
	{
		for (size_t i = 1; i < numMatrices; ++i)
		{
			auto parent = tree[i].second;
			tree[i].first = tree[parent].first * tree[i].first;
		}
	}
}

// Each matrix is allocated independently
// Pointers stored together with parent index
static void ScatteredMatrices(benchmark::State& state)
{
	// -- Set up ----
	const size_t numMatrices = state.range();
	using Node = std::pair<std::unique_ptr<Mat44f>, size_t>; // Mtx ptr, parent index
	// Allocate data
	// 	   We intentionally don´t reserve the vector so that it scatters memory around
	// 	   when it reallocates. This gives a more realistic memory distribution of the matrix allocations
	std::vector<Node> nodes;
	std::uniform_int_distribution<size_t> ints(0, numMatrices);
	for (size_t i = 0; i < numMatrices; ++i)
	{
		nodes.push_back({ std::make_unique<Mat44f>(), 0 });
		for (size_t j = 0; j < 16; ++j)
			nodes[i].first->data()[j] = reals(rng);
		nodes[i].second = i ? ints(rng) % i : 0;
	}
	
	// -- Run the actual benchmark ---
	while (state.KeepRunning())
	{
		for (size_t i = 1; i < numMatrices; ++i)
		{
			auto parent = nodes[i].second;
			*nodes[i].first = (*nodes[parent].first) * (*nodes[i].first);
		}
	}
}

BENCHMARK(ContiguousMatrixTree)
	->Arg(2 << 6) // Totally fits in L1 cache
	->Arg(2 << 8) // Totally fits in L1 cache
	->Arg(2 << 9) // Exact size of L1 cache
	->Arg(2 << 10) // Twice the size of L1 cache
	->Arg(2 << 12) // Totally fits in L2 size
	->Arg(2 << 14) // Exact L2 size
	->Arg(2 << 18) // Exact L3 size
	->Arg(2 << 24); // Main memory

BENCHMARK(InterleavedMatricesTree)
->Arg(2 << 6) // Totally fits in L1 cache
->Arg(2 << 8) // Totally fits in L1 cache
->Arg(2 << 9) // Exact size of L1 cache
->Arg(2 << 10) // Twice the size of L1 cache
->Arg(2 << 12) // Totally fits in L2 size
->Arg(2 << 14) // Exact L2 size
->Arg(2 << 18) // Exact L3 size
->Arg(2 << 24); // Main memory

BENCHMARK(ScatteredMatrices)
->Arg(2 << 6) // Totally fits in L1 cache
->Arg(2 << 8) // Totally fits in L1 cache
->Arg(2 << 9) // Exact size of L1 cache
->Arg(2 << 10) // Twice the size of L1 cache
->Arg(2 << 12) // Totally fits in L2 size
->Arg(2 << 14) // Exact L2 size
->Arg(2 << 18) // Exact L3 size
->Arg(2 << 24); // Main memory

BENCHMARK_MAIN();