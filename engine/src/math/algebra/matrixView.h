//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <cstddef>

namespace rev::math {

	template<typename T, size_t m, size_t n, class Derived>
	struct MatrixView
	{
		// Generic component accessor.
		T& operator()(size_t i, size_t j) { return static_cast<Derived&>(*this)(i, j); }

		// Component accessor for when you know the component index at compile time.
		template<size_t i, size_t j>
		T& coefficient()
		{
			static_assert(i < m);
			static_assert(j < n);
			auto& derived = static_cast<Derived&>(*this);
			return derived.template coefficient<i, j>();
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

		template<size_t i0, size_t j0, size_t rows, size_t cols>
		class Block : MatrixView<T, rows, cols, Block<i0, j0, rows, cols>>
		{
		private:
			MatrixView<T, m, n, Derived>& parentExpr() const
			{
				return *reinterpret_cast<MatrixView<T, m, n, Derived>*>(this);
			}

		public:
			// Generic component accessor.
			T& operator()(size_t i, size_t j) {
				return parentExpr()(i0 + i, j0 + j);
			}

			// Component accessor for when you know the component index at compile time.
			template<size_t i, size_t j>
			T& coefficient()
			{
				static_assert(i < rows);
				static_assert(j < cols);
				return parentExpr().template coefficient<i0 + i, j0 + j>();
			}
		};

		template<size_t i0, size_t j0, size_t rows, size_t cols>
		Block<i0, j0, rows, cols>& block()
		{
			static_assert(i0 + rows <= m);
			static_assert(j0 + cols <= n);
			return *reinterpret_cast<const Block<i0, j0, rows, cols>*>(this);
		}

		template<size_t i>
		Block<i, 0, 1, n>& row()
		{
			return block<i, 0, 1, n>();
		}

		template<size_t j>
		Block<0, j, m, 1>& col()
		{
			return block<0, j, m, 1>();
		}
	};
}