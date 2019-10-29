//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <cstddef>
#include <functional>

namespace rev::math {

	template<typename T, size_t m, size_t n, class Derived>
	struct MatrixExpr
	{
		// Generic component accessor.
		T operator()(size_t i, size_t j) const { return static_cast<const Derived&>(*this)(i, j); }

		// Block access
		template<size_t rows, size_t cols, size_t i0, size_t j0>
		struct Block : MatrixExpr<T, rows, cols, Block<rows, cols, i0, j0>>
		{
		private:
			MatrixExpr<T, m, n, Derived>& parentExpr() const
			{
				return *reinterpret_cast<const MatrixExpr<T, m, n, Derived>*>(this);
			}

		public:
			// Generic component accessor.
			T operator()(size_t i, size_t j) const {
				return parentExpr()(i0 + i, j0 + j);
			}
		};

		template<size_t rows, size_t cols, size_t i0, size_t j0>
		const Block<rows, cols, i0, j0>& block() const
		{
			static_assert(i0 + rows <= m);
			static_assert(j0 + cols <= n);
			return *reinterpret_cast<const Block<i0, j0, rows, cols>*>(this);
		}

		template<size_t i>
		auto& row() const
		{
			return block<1, n, i, 0>();
		}

		template<size_t j>
		auto& col() const
		{
			return block<m, 1, 0, j>();
		}

		// Transpose
		struct Transpose : MatrixExpr<T, n, m, Transpose>
		{
			// Generic component accessor.
			T operator()(size_t i, size_t j) const {
				return (*reinterpret_cast<const MatrixExpr<T, m, n, Derived>*>(this))(j,i);
			}
		};

		const Transpose& transpose() const
		{
			return *reinterpret_cast<const Transpose*>(this);
		}
	};

	template<class T, size_t m, size_t n, class A, class B, class Op>
	struct CWiseMatrixBinaryOp : MatrixExpr<T, m, n, CWiseMatrixBinaryOp<T, m, n, A, B, Op>>
	{
		const MatrixExpr<T, m, n, A>& a;
		const MatrixExpr<T, m, n, B>& b;

		CWiseMatrixBinaryOp(const MatrixExpr<T, m, n, A>& _a, const MatrixExpr<T, m, n, B>& _b)
			:a(_a), b(_b)
		{}

		// Generic component accessor.
		T operator()(size_t i, size_t j) const
		{
			Op op;
			return op(a(i, j), b(i, j));
		}
	};

	template<class T, size_t m, size_t n, class A, class B>
	auto operator+(const MatrixExpr<T, m, n, A>& a, const MatrixExpr<T, m, n, B>& b)
	{
		return CWiseMatrixBinaryOp<T, m, n, A, B, std::plus<T>>(a, b);
	}

	template<class T, size_t m, size_t n, class A, class B>
	auto operator-(const MatrixExpr<T, m, n, A>& a, const MatrixExpr<T, m, n, B>& b)
	{
		return CWiseMatrixBinaryOp<T, m, n, A, B, std::minus<T>>(a, b);
	}

	template<class T, size_t m, size_t n, class A, class B>
	auto max(const MatrixExpr<T, m, n, A>& a, const MatrixExpr<T, m, n, B>& b)
	{
		return CWiseMatrixBinaryOp<T, m, n, A, B, math::max<T>>(a, b);
	}

	template<class T, size_t m, size_t n, class A, class B>
	auto min(const MatrixExpr<T, m, n, A>& a, const MatrixExpr<T, m, n, B>& b)
	{
		return CWiseMatrixBinaryOp<T, m, n, A, B, math::min<T>>(a, b);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class T, size_t m, size_t k, size_t n, class A, class B>
	struct MatrixProduct : MatrixExpr<T,m,n,MatrixProduct<T,m,k,n,A,B>>
	{
		const MatrixExpr<T, m, k, A>& a;
		const MatrixExpr<T, k, n, B>& b;

		MatrixProduct(const MatrixExpr<T, m, k, A>& _a, const MatrixExpr<T, k, n, B>& _b)
			:a(_a), b(_b)
		{}

		// Generic component accessor.
		T operator()(size_t i, size_t j) const
		{
			T accum = a(i, 0) * b(0, j);
			for (size_t l = 1; l < k ++l)
				accum += a(i, l) * b(l, j);
			return accum;
		}
	};

	template<class T, size_t m, size_t k, size_t n, class A, class B>
	MatrixProduct<T, m, k, n, A, B> operator*(const MatrixExpr<T, m, k, A>& a, const MatrixExpr<T, k, n, B>& b)
	{
		return MatrixProduct<T, m, k, n, A, B>(a, b);
	}
}