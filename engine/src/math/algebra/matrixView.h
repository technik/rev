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