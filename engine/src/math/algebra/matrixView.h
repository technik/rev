//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <cstddef>
#include "matrixExpr.h"

namespace rev::math {

	template<typename T, size_t m, size_t n, class Derived>
	struct MatrixView : MatrixExpr<T,m,n,Derived>
	{
		// Generic component accessor.
		T& operator()(size_t i, size_t j) { return static_cast<Derived&>(*this)(i, j); }

		// Vector component accessor
		T& operator[](size_t i)
		{
			static_assert(n == 1);
			return static_cast<Derived&>(*this)(i,0);
		}

		// Component accessor for when you know the component index at compile time.
		template<size_t i>
		T& getComponent() const
		{
			static_assert(n == 1);
			static_assert(i < m);
			auto& derived = static_cast<const Derived&>(*this);
			return derived.template getComponent<i>();
		}

		// Named accessors
		T& x() { static_assert(n == 1, "Expression is not a vector"); return (*this)(0,0); }
		T& y() { static_assert(n == 1, "Expression is not a vector"); return (*this)(1,0); }
		T& z() { static_assert(n == 1, "Expression is not a vector"); return (*this)(2,0); }
		T& w() { static_assert(n == 1, "Expression is not a vector"); return (*this)(3,0); }

		// Prevent method hiding
		using MatrixExpr<T, m, n, Derived>::x;
		using MatrixExpr<T, m, n, Derived>::y;
		using MatrixExpr<T, m, n, Derived>::z;
		using MatrixExpr<T, m, n, Derived>::w;
		using MatrixExpr<T, m, n, Derived>::getComponent;
		using MatrixExpr<T, m, n, Derived>::block;
		using MatrixExpr<T, m, n, Derived>::row;
		using MatrixExpr<T, m, n, Derived>::col;

		template<class Other>
		MatrixView& operator=(const MatrixExpr<T, m, n, Other>& x)
		{
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
					(*this)(i, j) = x(i, j);
			return *this;
		}

		void setIdentity()
		{
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
					(*this)(i, j) = (i == j) ? 1 : 0;
		}

		void setZero()
		{
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
					(*this)(i, j) = 0;
		}

		void setOnes()
		{
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
					(*this)(i, j) = 1;
		}

		template<size_t rows, size_t cols, size_t i0, size_t j0>
		struct BlockView : MatrixView<T, rows, cols, BlockView<rows, cols, i0, j0>>
		{
		private:
			MatrixView<T, m, n, Derived>& parentExpr() const
			{
				return *reinterpret_cast<MatrixView<T, m, n, Derived>*>(this);
			}

		public:
			using MatrixView<T, rows, cols, BlockView<rows, cols, i0, j0>>::operator=;
			// Generic component accessor.
			T& operator()(size_t i, size_t j) {
				return parentExpr()(i0 + i, j0 + j);
			}
		};

		template<size_t rows, size_t cols, size_t i0, size_t j0>
		auto& block()
		{
			static_assert(i0 + rows <= m);
			static_assert(j0 + cols <= n);
			return *reinterpret_cast<BlockView<rows, cols, i0, j0>*>(this);
		}

		template<size_t i>
		auto& row()
		{
			return block<1, n, i, 0>();
		}

		template<size_t j>
		auto& col()
		{
			return block<m, 1, 0, j>();
		}
	};
}