//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
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

namespace rev::math
{
	template<class T>
	auto lerp(const T& a, const T& b, float x)
	{
		return a*(1-x)+b*x;
	}

	inline float max(float a, float b)
	{
		return a>b?a:b; // Same NaN behavior as simd
	}

	inline float min(float a, float b)
	{
		return a<b?a:b; // Same NaN behavior as simd
	}

	inline float clamp(float t, float a, float b)
	{
		return max(a, min(b, t));
	}

	template<class T> struct minOp
	{
		T operator()(T a, T b) const { return (a < b) ? a : b; } // Same NaN behavior as simd
	};

	template<class T> struct maxOp
	{
		T operator()(T a, T b) const { return (a > b) ? a : b; } // Same NaN behavior as simd
	};
}
