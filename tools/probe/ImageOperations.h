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

#include <vector>
#include <graphics/Image.h>

// save the diff of each pixel (a-b) into aMinusB
void diff(const rev::gfx::Image3f& a, const rev::gfx::Image3f& b, rev::gfx::Image3f& aMinusB, float scale = 1.f);

// Copy the left half of left, and the right half of right into dst
void sideBySide(const rev::gfx::Image3f& left, const rev::gfx::Image3f& right, rev::gfx::Image3f& dst);

// Copy the top half of top, and the bottom half of bottom into dst
void topAndBottom(const rev::gfx::Image3f& top, const rev::gfx::Image3f& bottom, rev::gfx::Image3f& dst);

// Copy the images from the input array next each other into the output
void composeRow(const std::vector<std::shared_ptr<rev::gfx::Image3f>>& input, rev::gfx::Image3f& dst);

// Copy the images from the input array top to bottom into the output
void composeColumn(const std::vector<std::shared_ptr<rev::gfx::Image3f>>& input, rev::gfx::Image3f& dst);