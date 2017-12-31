//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Ag�era Tortosa (a.k.a. Technik)
// 2014/April/05
//----------------------------------------------------------------------------------------------------------------------
// Generic matrix
#ifndef _REV_MATH_ALGEBRA_MATRIX_H_
#define _REV_MATH_ALGEBRA_MATRIX_H_

#include <cstddef>
#include <algorithm>
#include <initializer_list>

namespace rev {
	namespace math {

		//------------------------------------------------------------------------------------------------------------------
		template<
			class T_,
			size_t rows_,
			size_t cols_,
			size_t row_stride_,
			size_t col_stride_,
			bool col_major_,
			typename Derived_>
		struct MatrixStorageBase {
			using Element = T_;
			static constexpr bool	is_col_major = col_major_;
			static constexpr size_t size = rows_*cols_;
			static constexpr size_t cols = cols_;
			static constexpr size_t rows = rows_;
			static constexpr size_t row_stride = row_stride_;
			static constexpr size_t col_stride = col_stride_;
			using Idx = size_t;
			using Derived = Derived_;

			T_& operator()(Idx i, Idx j)			{ return m[i*row_stride_+j*col_stride_]; }
			T_	operator()(Idx i, Idx j) const	{ return m[i*row_stride_+j*col_stride_]; }

			T_&			operator()	(size_t _i);
			T_			operator() 	(size_t _i) const;
			T_&			operator[]	(size_t _i);
			T_			operator[] 	(size_t _i) const;

		private:
			T_ m[rows_*cols_];
		};

		//------------------------------------------------------------------------------------------------------------------
		template<typename M1_, typename M2_>
		bool operator==(const M1_& _a, const M2_& _b)
		{
			for(auto j = 0; j < M1_::cols; ++j) {
				for(auto i = 0; i < M1_::rows; ++i) {
					if(_a(i,j) != _b(i,j))
						return false;
				}
			}
			return true;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename M1_, typename M2_>
		bool operator!=(const M1_& _a, const M2_& _b)
		{
			for(auto j = 0; j < M1_::cols_; ++j) {
				for(auto i = 0; i < M1_::rows_; ++i) {
					if(_a(i,j) != _b(i,j))
						return true;
				}
			}
			return false;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<
			class T_,
			size_t rows_,
			size_t cols_,
			bool col_major_,
			typename Derived_>
		using MatrixDenseStorage = MatrixStorageBase<
			T_,
			rows_,
			cols_,
			col_major_? 0 : cols_,
			col_major_? rows_ : 0,
			col_major_,
			Derived_
		>;

		//------------------------------------------------------------------------------------------------------------------
		template<size_t rows_, size_t cols_, typename Storage_>
		struct MatrixBase : public Storage_{
			static constexpr size_t rows = rows_;
			static constexpr size_t cols = cols_;
			static constexpr bool	is_col_major = Storage_::is_col_major;
			static constexpr size_t row_stride = Storage_::row_stride;
			static constexpr size_t col_stride = Storage_::col_stride;
			using Derived = typename Storage_::Derived;
			using Element = typename Storage_::Element;

			using TransposeStorage = MatrixStorageBase<Element,cols_,rows_,col_stride,row_stride,!is_col_major,Derived>;
			using TransposeView = MatrixBase<cols_,rows_,TransposeStorage>; // Keep memory layout, transpose access pattern

			template<size_t h_, size_t w_>
			using RegionDerived = MatrixBase<h_,w_,MatrixDenseStorage<Element,h_,w_,is_col_major,Derived>>;
			template<size_t h_, size_t w_>
			using RegionStorage = MatrixStorageBase<Element,h_,w_,row_stride,col_stride,is_col_major,RegionDerived<h_,w_>>;
			template<size_t h_, size_t w_>
			using RegionProxy = MatrixBase<h_,w_,RegionStorage<h_,w_>>;
			using ColumnProxy = RegionProxy<rows_,1>;
			using RowProxy = RegionProxy<1,cols_>;

			// Element access
			ColumnProxy&	col(size_t _i) const	{ return getProxy<ColumnProxy>(0,_i); }
			ColumnProxy&	col(size_t _i)			{ return getProxy<ColumnProxy>(0,_i); }
			RowProxy&		row(size_t _i) const	{ return getProxy<RowProxy>(_i,0); }
			RowProxy&		row(size_t _i)			{ return getProxy<RowProxy>(_i,0); }

			Element&	x()			{ return namedElement<0>(); }
			Element		x()	const	{ return namedElement<0>(); }
			Element&	y()			{ return namedElement<1>(); }
			Element		y()	const	{ return namedElement<1>(); }
			Element&	z()			{ return namedElement<2>(); }
			Element		z()	const	{ return namedElement<2>(); }
			Element&	w()			{ return namedElement<3>(); }
			Element		w()	const	{ return namedElement<3>(); }

			// Matrix operations
			Element			trace		() const;
			TransposeView&	transpose	() const;
			MatrixBase		inverse		() const;

			// Component wise operations
			template<typename OtherM_>
			Derived cwiseProduct(const OtherM_& _b);
			template<typename OtherM_>
			Derived cwiseMax	(const OtherM_& _b) const;
			template<typename OtherM_>
			Derived cwiseMin	(const OtherM_& _b) const;

			template<typename Other_>
			Element	dot(const Other_& _other) const;

			Derived abs			() const;
			Derived operator-	() const;

			// Basic modifiers
			static void setIdentity();
			static void setConstant(Element);
			static void setZero() { setConstant(T_(0)); }
			static void setOnes() { setConstant(T_(1)); }

		private:
			template<typename Proxy> Proxy& getProxy(size_t i, size_t j) { return *reinterpret_cast<Proxy*>(&(*this)(i,j)); }
			template<size_t n_>
			Element& namedElement() { 
				static_assert(cols>n_||rows>n_, "Only vectors have named element access"); return (*this)(n_); 
			}
			template<size_t n_>
			Element  namedElement() const { 
				static_assert(cols>n_||rows>n_, "Only vectors have named element access"); return (*this)(n_); 
			}
		};

		//------------------------------------------------------------------------------------------------------------------
		// Visitor expressions
		//------------------------------------------------------------------------------------------------------------------
		template<typename Matrix_, typename Operator_, bool col_major_ = Matrix_::is_col_major>
		auto cwiseUnaryOperator(
			Matrix_& _m,
			Operator_& _operation
		) -> typename Matrix_::Derived
		{
			Matrix_::Derived result;
			for(size_t j = 0; j < Matrix_::rows; ++j)
				for(size_t i = 0; i < Matrix_::cols; ++i)
					_operation(result(i,j),_m(i,j));
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename MatrixA_, typename MatrixB_, typename Operator_>
		auto cwiseBinaryOperator(
			MatrixA_& _a,
			MatrixB_& _b,
			Operator_& _operation
		) -> typename MatrixA_::Derived
		{
			MatrixA_::Derived result;
			for(size_t j = 0; j < MatrixA_::rows; ++j)
				for(size_t i = 0; i < MatrixA_::cols; ++i)
					_operation(result(i,j),_a(i,j),_b(i,j));
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename MatA_, typename MatB_>
		auto operator+(
			const MatA_& _a,
			const MatB_& _b
			) -> typename MatA_::Derived
		{
			using T = typename MatA_::Element;
			return cwiseBinaryOperator(_a, _b, [](T& res, T a, T b) { res = a+b; });
		}

		//------------------------------------------------------------------------------------------------------------------
		template<typename MatA_, typename MatB_>
		auto operator-(
			const MatA_& _a,
			const MatB_& _b
			) -> typename MatA_::Derived
		{
			using T = typename MatA_::Element;
			return cwiseBinaryOperator(_a, _b, [](T& res, T a, T b) { res = a-b; });
		}

		//------------------------------------------------------------------------------------------------------------------
		template<size_t m_, size_t n_, typename S_>
		auto operator*(
			const MatrixBase<m_,n_,S_>& _a,
			typename S_::Element _k
			) -> typename S_::Derived
		{
			using T = typename S_::Element;
			return cwiseUnaryOperator(_a, [=](typename T& res, typename T a) { res = a*_k; });
		}

		//------------------------------------------------------------------------------------------------------------------
		template<size_t m_, size_t n_, typename S_>
		auto operator*(
			typename S_::Element _k,
			const MatrixBase<m_,n_,S_>& _a
			) -> typename S_::Derived
		{
			return _a*_k;
		}

		//------------------------------------------------------------------------------------------------------------------
		template<class T_, size_t rows_, size_t cols_, bool col_major_ = true>
		struct Matrix
			: public MatrixBase
			<
				rows_,
				cols_,
				MatrixDenseStorage
				<
					T_,
					rows_,
					cols_,
					col_major_,
					Matrix<T_, rows_, cols_, col_major_>
				>
			>
		{
			using Base = MatrixBase<
				rows_,
				cols_,
				MatrixDenseStorage
				<
					T_,
					rows_,
					cols_,
					col_major_,
					Matrix
				>
			>;
			using Derived = Matrix;

			// Basic construction
			Matrix() = default;
			Matrix(const Matrix&) = default;
			Matrix(std::initializer_list<T_> _l) {
				auto iter = _l.begin();
				for(size_t i = 0; i < rows; ++i)
					for(size_t j = 0; j < cols; ++j) {
						(*this)(i,j) = *iter++;
					}
			}
			Matrix(T_ _x, T_ _y)
				: Base {_x, _y}
			{
				static_assert(cols==1||rows==1, "This constructor is for vectors only");
				static_assert(cols>=2||rows>=2, "Vector is not big enough");
			}

			Matrix(T_ _x, T_ _y, T_ _z)
				: Base { _x, _y, _z }
			{
				static_assert(cols==1||rows==1, "This constructor is for vectors only");
				static_assert(cols>=3||rows>=3, "Vector is not big enough");
			}

			// Smarter construction
			static constexpr Matrix identity();
			static constexpr Matrix zero();
			static constexpr Matrix ones();
		};

		//------------------------------------------------------------------------------------------------------------------
		template<size_t m_, size_t n_, size_t l_, typename S1_, typename S2_>
		auto operator*(
			const MatrixBase<m_,n_,S1_>& _a,
			const MatrixBase<n_,l_,S2_>& _b
			) -> typename Matrix<typename S1_::Element,m_,l_,S1_::is_col_major>
		{
			using Result = Matrix<typename S1_::Element,m_,l_,S1_::is_col_major>;
			Result result;
			for(auto i = 0; i < m_; ++i) { // for each row in _a
				for(auto k = 0; k < l_; ++k) { // for each column in _b
					result(i,k) = Result::Element(0);
					for(auto j = 0; j < n_; ++j) { // for each element
						result(i,j) += _a(i,j) * _b(j,k);
					}
				}
			}
			return result;
		}

		//------------------------------------------------------------------------------------------------------------------
		// Useful aliases
		template<typename T_>
		using Matrix22 = Matrix<T_,2,2>;
		template<typename T_>
		using Matrix33 = Matrix<T_,3,3>;
		template<typename T_>
		using Matrix34 = Matrix<T_,3,4>;
		template<typename T_>
		using Matrix44 = Matrix<T_,4,4>;

		using Mat22f = Matrix22<float>;
		using Mat33f = Matrix33<float>;
		using Mat34f = Matrix34<float>;
		using Mat44f = Matrix44<float>;
	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_MATRIX_H_