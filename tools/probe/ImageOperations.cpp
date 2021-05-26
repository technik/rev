//--------------------------------------------------------------------------------------------------
// Revolution Engine
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
#pragma once

#include <cassert>
#include "ImageOperations.h"

using std::vector;
using std::shared_ptr;
using namespace rev::gfx;

// save the diff of each pixel (a-b) into aMinusB
void diff(const Image3f& a, const Image3f& b, Image3f& aMinusB, float scale)
{
	assert(a.size() == b.size());
	assert(a.size() == aMinusB.size());

	for (uint32_t i = 0; i < a.height(); ++i)
	{
		for (uint32_t j = 0; j < a.width(); ++j)
		{
			aMinusB.pixel(j, i) = (a.pixel(j, i) - b.pixel(j, i)) * scale;
		}
	}
}

// Copy the left half of left, and the right half of right into dst
void sideBySide(const Image3f& left, const Image3f& right, Image3f& dst)
{
	assert(left.size() == right.size());
	assert(left.size() == dst.size());

	const auto midSize = dst.width() / 2;

	// Copy left side
	for (uint32_t i = 0; i < dst.height(); ++i)
	{
		for (uint32_t j = 0; j < midSize; ++j)
		{
			dst.pixel(j, i) = left.pixel(j, i);
		}
	}

	// Copy right side
	for (uint32_t i = 0; i < dst.height(); ++i)
	{
		for (uint32_t j = midSize; j < dst.width(); ++j)
		{
			dst.pixel(j, i) = right.pixel(j, i);
		}
	}
}

// Copy the top half of top, and the bottom half of bottom into dst
void topAndBottom(const Image3f& top, const Image3f& bottom, Image3f& dst)
{
	assert(top.size() == bottom.size());
	assert(top.size() == dst.size());

	const auto midSize = dst.height() / 2;

	// Copy top side
	for (uint32_t i = 0; i < midSize; ++i)
	{
		for (uint32_t j = 0; j < dst.width(); ++j)
		{
			dst.pixel(j, i) = top.pixel(j, i);
		}
	}

	// Copy bottom side
	for (uint32_t i = midSize; i < dst.height(); ++i)
	{
		for (uint32_t j = 0; j < dst.width(); ++j)
		{
			dst.pixel(j, i) = bottom.pixel(j, i);
		}
	}
}

// Copy the images from the input array next each other into the output
void composeRow(const vector<shared_ptr<Image3f>>& input, Image3f& dst)
{
	if (input.empty())
		return;

	assert(input[0]->height() == dst.height());
	assert(input[0]->width() * input.size() == dst.width());

	const uint32_t stride = input[0]->width();

	for (uint32_t n = 0; n < input.size(); ++n)
	{
		auto& src = *input[n];

		for (uint32_t i = 0; i < src.height(); ++i)
		{
			for (uint32_t j = 0; j < src.width(); ++j)
			{
				dst.pixel(j + n*stride, i) = src.pixel(j, i);
			}
		}
	}
}

// Copy the images from the input array top to bottom into the output
void composeColumn(const vector<shared_ptr<Image3f>>& input, Image3f& dst)
{
	if (input.empty())
		return;

	assert(input[0]->width() == dst.width());
	assert(input[0]->height() * input.size() == dst.height());

	const uint32_t stride = input[0]->height();

	for (uint32_t n = 0; n < input.size(); ++n)
	{
		auto& src = *input[n];

		for (uint32_t i = 0; i < dst.height(); ++i)
		{
			for (uint32_t j = 0; j < dst.width(); ++j)
			{
				dst.pixel(j, i + n * stride) = src.pixel(j, i);
			}
		}
	}
}