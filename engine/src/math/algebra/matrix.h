//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Generic matrix
#ifndef _REV_MATH_ALGEBRA_MATRIX_H_
#define _REV_MATH_ALGEBRA_MATRIX_H_

#include <cstddef>
#include <algorithm>

namespace rev {
	namespace math {

		template<class T_, size_t rows_, size_t cols_>
		struct Matrix {
			using Element = T_;
			static constexpr size_t rows = rows_;
			static constexpr size_t cols = cols_;

			// Smarter construction
			static constexpr Matrix identity();
			static constexpr Matrix zero();
			static constexpr Matrix ones();

			// Element access
			Element&		operator()	(size_t row, size_t col)		{ return m[row][col]; }
			const Element&	operator()	(size_t row, size_t col) const	{ return m[row][col]; }

			// Component wise operations
			template<typename Operator_>
			Matrix cwiseOperator(const Matrix& _b, const Operator_& _operation) const;
			Matrix cwiseProduct(const Matrix& _b) const { return cwiseOperator(_b,[](T_ a, T_ b){ return a*b; }); }
			Matrix cwiseMax(const Matrix& _b) const { return cwiseOperator(_b,[](T_ a, T_ b)->T_ { return std::max(a,b);}); }
			Matrix cwiseMin(const Matrix& _b) const { return cwiseOperator(_b,[](T_ a, T_ b)->T_ { return std::min(a,b);}); }

			// Visitor operations
			template<typename Visitor_>
			Matrix visitorOperator(const Visitor_& _visitor) const;
			Matrix abs() const { return visitorOperator([](T_ a){ return std::abs(a); }); }
			Matrix operator-() const { return visitorOperator([](T_ a){ return -a; }); }

		private:
			T_ m[rows][cols]; ///< Storage, column-major
		};

		//------------------------------------------------------------------------------------------------------------------
		// Inline implementation
		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		constexpr Matrix<T_, rows_, cols_> Matrix<T_, rows_, cols_>::identity() {
			static_assert(rows_==cols_, "Matrix must be square. Otherwise identity isn't well defined");
			Matrix result;
			for(auto j = 0; j < cols; ++j) {
				for(auto i = 0; i < rows; ++i) {
					result(i,j) = T_(i==j? 1 : 0);
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		constexpr Matrix<T_, rows_, cols_> Matrix<T_, rows_, cols_>::zero() {
			Matrix result;
			for(auto j = 0; j < cols; ++j) {
				for(auto i = 0; i < rows; ++i) {
					result(i,j) = T_(0);
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		constexpr Matrix<T_, rows_, cols_> Matrix<T_, rows_, cols_>::ones() {
			Matrix result;
			for(auto j = 0; j < cols; ++j) {
				for(auto i = 0; i < rows; ++i) {
					result(i,j) = T_(1);
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		// External operators
		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		Matrix<T_,rows_,cols_> operator+(
			const Matrix<T_,rows_,cols_>& _a,
			const Matrix<T_,rows_,cols_>& _b
			)
		{
			return _a.cwiseOperator(_b, [](T_ a, T_ b) { return a+b; });
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		Matrix<T_,rows_,cols_> operator-(
			const Matrix<T_,rows_,cols_>& _a,
			const Matrix<T_,rows_,cols_>& _b
			)
		{
			return _a.cwiseOperator(_b, [](T_ a, T_ b) { return a-b; });
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t m_, size_t n_, size_t l_>
		Matrix<T_,m_,l_> operator*(
			const Matrix<T_,m_,n_>& _a,
			const Matrix<T_,n_,l_>& _b
			)
		{
			Matrix<T_,m_,l_> result;
			for(auto i = 0; i < m_; ++i) { // for each row in _a
				for(auto k = 0; k < l_; ++i) { // for each column in _b
					result(i,k) = T_(0);
					for(auto j = 0; j < n_; ++j) { // for each element
						result(i,j) += _a(i,j) * _b(j,k);
					}
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		Matrix<T_, rows_, cols_> operator*(const Matrix<T_, rows_, cols_>& m, T_ k)
		{
			Matrix<T_, rows_, cols_> result;
			for(auto j = 0; j < cols_; ++j) {
				for(auto i = 0; i < rows_; ++i) {
					result(i,j) = m(i,j) * k;
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		Matrix<T_, rows_, cols_> operator*(T_ k, const Matrix<T_, rows_, cols_>& m)
		{
			return m*k;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		template<typename Operator_>
		auto Matrix<T_, rows_, cols_>::cwiseOperator(
			const Matrix<T_, rows_, cols_>& _b,
			const Operator_& _operation
		) const -> Matrix
		{
			Matrix result;
			for(auto j = 0; j < cols; ++j) {
				for(auto i = 0; i < rows; ++i) {
					result(i,j) = _operation((*this)(i,j), _b(i,j));
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		template<typename Visitor_>
		auto Matrix<T_, rows_, cols_>::visitorOperator(
			const Visitor_& _operation
		) const -> Matrix
		{
			Matrix result;
			for(auto j = 0; j < cols; ++j) {
				for(auto i = 0; i < rows; ++i) {
					result(i,j) = _operation((*this)(i,j));
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_>
		bool operator==(const Matrix<T_,rows_,cols_>& _a, const Matrix<T_,rows_,cols_>& _b)
		{
			for(auto j = 0; j < cols_; ++j) {
				for(auto i = 0; i < rows_; ++i) {
					if(_a(i,j) != _b(i,j))
						return false;
				}
			}
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------
		// Useful aliases
		using Mat22f = Matrix<float, 2, 2>;
		using Mat33f = Matrix<float, 3, 3>;
		using Mat44f = Matrix<float, 4, 4>;
	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_MATRIX_H_