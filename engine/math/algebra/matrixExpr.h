//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <cstddef>
#include <functional>
#include "../linear.h"

namespace rev::math {

	template<class T, size_t m, size_t n, class A, class B, class Op>
	struct CWiseMatrixBinaryOp;

	template<typename T, size_t m, size_t n, class Derived>
	struct MatrixExpr
	{
		// Generic component accessor.
		T operator()(size_t i, size_t j) const
		{
			return static_cast<const Derived&>(*this)(i, j);
		}

		template<class Other>
		bool operator==(const MatrixExpr<T, m, n, Other>& x) const
		{
			for (size_t i = 0; i < m; ++i)
				for (size_t j = 0; j < n; ++j)
					if (x(i, j) != (*this)(i, j))
						return false;
			return true;
		}

		template<class Other>
		bool operator!=(const MatrixExpr<T, m, n, Other>& x) const
		{
			return !(*this == x);
		}
		
		// Vector component accessor
		T operator[](size_t i) const
		{
			static_assert(n == 1);
			return static_cast<const Derived&>(*this)(i,0);
		}

		// Component accessor for when you know the component index at compile time.
		template<size_t i>
		T  getComponent() const
		{
			static_assert(n == 1);
			static_assert(i < m);
			auto& derived = static_cast<const Derived&>(*this);
			return derived.template getComponent<i>();
		}

		// Named accessors
		T	x()	const { static_assert(n == 1, "Expression is not a vector"); return (*this)(0,0); }
		T	y()	const { static_assert(n == 1, "Expression is not a vector"); return (*this)(1,0); }
		T	z()	const { static_assert(n == 1, "Expression is not a vector"); return (*this)(2,0); }
		T	w()	const { static_assert(n == 1, "Expression is not a vector"); return (*this)(3,0); }

		// Block access
		template<size_t rows, size_t cols, size_t i0, size_t j0>
		struct BlockExpr : MatrixExpr<T, rows, cols, BlockExpr<rows, cols, i0, j0>>
		{
		private:
			auto& parentExpr() const
			{
				return reinterpret_cast<const MatrixExpr<T, m, n, Derived>&>(*this);
			}

		public:
			// Generic component accessor.
			T operator()(size_t i, size_t j) const {
				return parentExpr()(i0 + i, j0 + j);
			}
		};

		template<size_t rows, size_t cols, size_t i0, size_t j0>
		const auto& block() const
		{
			static_assert(i0 + rows <= m);
			static_assert(j0 + cols <= n);
			return *reinterpret_cast<const BlockExpr<rows, cols, i0, j0>*>(this);
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

		auto& xy() const
		{
			static_assert(n == 1 && m >= 2);
			return block<2,1,0,0>();
		}

		auto& xyz() const
		{
			static_assert(n == 1 && m >= 3);
			return block<3,1,0,0>();
		}

		// Constant matrix expression
		template<int value>
		struct LiteralExpr : MatrixExpr<T, m, n, LiteralExpr<value>>
		{
			// Generic component accessor.
			T operator()(size_t, size_t) const {
				return T(value);
			}
		};

		static auto zero() { return LiteralExpr<0>(); }
		static auto ones() { return LiteralExpr<1>(); }

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

		template<class Op>
		struct CWiseUnaryExpr : MatrixExpr<T, m, n, CWiseUnaryExpr<Op>>
		{
			// Generic component accessor.
			T operator()(size_t i, size_t j) const
			{
				auto parent = reinterpret_cast<const MatrixExpr*>(this);
				Op op;
				return op((*parent)(i,j));
			}
		};

		template<class Other, class Op>
		struct CWiseMatrixBinaryOp : MatrixExpr<T, m, n, CWiseMatrixBinaryOp<Other, Op>>
		{
			const MatrixExpr<T, m, n, Derived>& a;
			const MatrixExpr<T, m, n, Other>& b;

			CWiseMatrixBinaryOp(const MatrixExpr<T, m, n, Derived>& _a, const MatrixExpr<T, m, n, Other>& _b)
				:a(_a), b(_b)
			{}

			// Generic component accessor.
			T operator()(size_t i, size_t j) const
			{
				Op op;
				return op(a(i, j), b(i, j));
			}
		};

		struct ProductByScalarExpr : MatrixExpr<T, m, n, ProductByScalarExpr>
		{
			T k;
			ProductByScalarExpr(const MatrixExpr<T, m, n, Derived>& _x, T _k)
				:ref(_x), k(_k)
			{}

			// Generic component accessor.
			T operator()(size_t i, size_t j) const
			{
				return ref(i, j) * k;
			}
		private:
			const MatrixExpr<T, m, n, Derived>& ref;
		};

		struct DivideByScalarExpr : MatrixExpr<T, m, n, DivideByScalarExpr>
		{
			T k;
			DivideByScalarExpr(const MatrixExpr<T, m, n, Derived>& _x, T _k)
				:ref(_x), k(_k)
			{}

			// Generic component accessor.
			T operator()(size_t i, size_t j) const
			{
				return ref(i, j) / k;
			}
		private:
			const MatrixExpr<T, m, n, Derived>& ref;
		};

		template<class Other>
		auto cwiseProduct(const MatrixExpr < T, m, n, Other>& x) const
		{
			return CWiseMatrixBinaryOp<Other, std::multiplies<T>>(*this, x);
		}
	};

	template<class T, size_t m, size_t n, class Derived>
	auto& operator-(const MatrixExpr<T,m,n,Derived>& x)
	{
		return reinterpret_cast<const MatrixExpr<T, m, n, Derived>::template CWiseUnaryExpr<std::negate<T>>&>(x);
	}

	template<class T, size_t m, size_t n, class Derived>
	auto operator*(const MatrixExpr<T, m, n, Derived>& x, T k)
	{
		return MatrixExpr<T, m, n, Derived>::ProductByScalarExpr(x, k);
	}

	template<class T, size_t m, size_t n, class Derived>
	auto operator*(T k, const MatrixExpr<T, m, n, Derived>& x)
	{
		return MatrixExpr<T, m, n, Derived>::ProductByScalarExpr(x, k);
	}

	template<class T, size_t m, size_t n, class Derived>
	auto operator/(const MatrixExpr<T, m, n, Derived>& x, T k)
	{
		return MatrixExpr<T, m, n, Derived>::DivideByScalarExpr(x, k);
	}

	template<class T, size_t m, size_t n, class A, class B>
	auto operator+(const MatrixExpr<T, m, n, A>& a, const MatrixExpr<T, m, n, B>& b)
	{
		return MatrixExpr<T, m, n, A>::template CWiseMatrixBinaryOp<B, std::plus<T>>(a, b);
	}

	template<class T, size_t m, size_t n, class A, class B>
	auto operator-(const MatrixExpr<T, m, n, A>& a, const MatrixExpr<T, m, n, B>& b)
	{
		return MatrixExpr<T, m, n, A>::template CWiseMatrixBinaryOp<B, std::minus<T>>(a, b);
	}

	template<class T, size_t m, size_t n, class A, class B>
	auto max(const MatrixExpr<T, m, n, A>& a, const MatrixExpr<T, m, n, B>& b)
	{
		return MatrixExpr<T, m, n, A>::template CWiseMatrixBinaryOp<B, math::maxOp<T>>(a, b);
	}

	template<class T, size_t m, size_t n, class A, class B>
	auto min(const MatrixExpr<T, m, n, A>& a, const MatrixExpr<T, m, n, B>& b)
	{
		return MatrixExpr<T, m, n, A>::template CWiseMatrixBinaryOp<B, math::minOp<T>>(a, b);
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
			for (size_t l = 1; l < k; ++l)
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