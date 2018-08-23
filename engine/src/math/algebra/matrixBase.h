//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
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
// Generic matrix
#pragma once

#include <cstddef>
#include <algorithm>
#include <initializer_list>
#include <cmath>

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
            static constexpr size_t size = rows_ * cols_;
            static constexpr size_t cols = cols_;
            static constexpr size_t rows = rows_;
            static constexpr size_t row_stride = row_stride_;
            static constexpr size_t col_stride = col_stride_;
            using Idx = size_t;
            using Derived = Derived_;

            MatrixStorageBase() = default;
            MatrixStorageBase(const MatrixStorageBase&) = default;
            MatrixStorageBase(std::initializer_list<T_> _l)
            {
                auto iter = _l.begin();
                for (size_t i = 0; i < rows_; ++i)
                    for (size_t j = 0; j < cols_; ++j)
                        (*this)(i, j) = *iter++;
            }

            template<
                size_t rowsB_,
                size_t colsB_,
                size_t row_strideB_,
                size_t col_strideB_,
                bool col_majorB_,
                typename DerivedB_>
                MatrixStorageBase(
                    const MatrixStorageBase<T_, rowsB_, colsB_, row_strideB_, col_strideB_, col_majorB_, DerivedB_>& _b)
            {
                for (size_t j = 0; j < cols_; ++j)
                    for (size_t i = 0; i < rows_; ++i)
                        (*this)(i, j) = _b(i, j);
            }

            T_&			coefficient(Idx i, Idx j)
            {
                auto i_rs = i * row_stride_;
                auto j_cs = j * col_stride_;
                return m[i_rs + j_cs];
                //return m[i*row_stride_+j*col_stride_]; 
            }
            const T_&	coefficient(Idx i, Idx j) const { return m[i*row_stride_ + j * col_stride_]; }
            T_&			operator()(Idx i, Idx j) { return coefficient(i, j); }
            const T_&	operator()(Idx i, Idx j) const { return coefficient(i, j); }

            T_&			coefficient(size_t i) { static_assert(cols == 1 || rows == 1, "This accessor is only for vectors"); return m[cols == 1 ? i * row_stride : i * col_stride]; }
            const T_&	coefficient(size_t i) const { static_assert(cols == 1 || rows == 1, "This accessor is only for vectors"); return m[cols == 1 ? i * row_stride : i * col_stride]; }
            T_&			operator()	(size_t i) { return coefficient(i); }
            const T_&	operator() 	(size_t i) const { return coefficient(i); }
            T_&			operator[]	(size_t i) { return coefficient(i); }
            const T_&	operator[] 	(size_t i) const { return coefficient(i); }

            const T_*	data() const { return m; }
            T_*			data() { return m; }

            private:
                T_ m[rows_*cols_];
        };

        //------------------------------------------------------------------------------------------------------------------
        template<
            class T_,
            size_t rows_,
            size_t cols_,
            size_t row_stride_A,
            size_t col_stride_A,
            bool col_major_A,
            typename Derived_A,
            size_t row_stride_B,
            size_t col_stride_B,
            bool col_major_B,
            typename Derived_B>
            bool operator==(
                const MatrixStorageBase<T_, rows_, cols_, row_stride_A, col_stride_A, col_major_A, Derived_A>& _a,
                const MatrixStorageBase<T_, rows_, cols_, row_stride_B, col_stride_B, col_major_B, Derived_B>& _b)
        {
            for (auto j = 0; j < cols_; ++j)
            {
                for (auto i = 0; i < rows_; ++i)
                {
                    if (_a(i, j) != _b(i, j))
                        return false;
                }
            }
            return true;
        }

        //------------------------------------------------------------------------------------------------------------------
        template<
            class T_,
            size_t rows_,
            size_t cols_,
            size_t row_stride_A,
            size_t col_stride_A,
            bool col_major_A,
            typename Derived_A,
            size_t row_stride_B,
            size_t col_stride_B,
            bool col_major_B,
            typename Derived_B>
            bool operator!=(
                const MatrixStorageBase<T_, rows_, cols_, row_stride_A, col_stride_A, col_major_A, Derived_A>& _a,
                const MatrixStorageBase<T_, rows_, cols_, row_stride_B, col_stride_B, col_major_B, Derived_B>& _b)
        {
            for (auto j = 0; j < cols_; ++j)
            {
                for (auto i = 0; i < rows_; ++i)
                {
                    if (_a(i, j) != _b(i, j))
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
            col_major_ ? 1 : cols_,
            col_major_ ? rows_ : 1,
            col_major_,
            Derived_
            >;

        //------------------------------------------------------------------------------------------------------------------
        template<size_t rows_, size_t cols_, typename Storage_>
        struct MatrixBase : public Storage_ {
            static constexpr size_t rows = rows_;
            static constexpr size_t cols = cols_;
            static constexpr size_t size = rows * cols;
            static constexpr bool	is_col_major = Storage_::is_col_major;
            static constexpr size_t row_stride = Storage_::row_stride;
            static constexpr size_t col_stride = Storage_::col_stride;
            using Derived = typename Storage_::Derived;
            using Element = typename Storage_::Element;

            using TransposeStorage = MatrixStorageBase<Element, cols_, rows_, col_stride, row_stride, !is_col_major, Derived>;
            using TransposeView = MatrixBase<cols_, rows_, TransposeStorage>; // Keep memory layout, transpose access pattern

            template<size_t h_, size_t w_>
            using RegionDerived = MatrixBase<h_, w_, MatrixDenseStorage<Element, h_, w_, is_col_major, Derived>>;
            template<size_t h_, size_t w_>
            using RegionStorage = MatrixStorageBase<Element, h_, w_, row_stride, col_stride, is_col_major, RegionDerived<h_, w_>>;
            template<size_t h_, size_t w_>
            using RegionProxy = MatrixBase<h_, w_, RegionStorage<h_, w_>>;
            using ColumnProxy = RegionProxy<rows_, 1>;
            using RowProxy = RegionProxy<1, cols_>;

            MatrixBase() = default;
            MatrixBase(const MatrixBase&) = default;
            MatrixBase(std::initializer_list<Element> _l)
                : Storage_(_l)
            {
            }

            template<typename Other_>
            MatrixBase(const Other_& _x)
                : Storage_(_x)
            {
            }

            MatrixBase& operator=(const MatrixBase& _other) = default;

            template<typename Other_>
            MatrixBase& operator=(const Other_& _x)
            {
                for (auto j = 0; j < cols_; ++j)
                {
                    for (auto i = 0; i < rows_; ++i)
                    {
                        Storage_::coefficient(i, j) = _x(i, j);
                    }
                }
                return *this;
            }

            // Element access
            template<size_t h_, size_t w_>
            const RegionProxy<h_, w_>&
                block(size_t i0, size_t j0) const
            {
                return getProxy<RegionProxy<h_, w_>>(i0, j0);
            }

            template<size_t h_, size_t w_>
            RegionProxy<h_, w_>&
                block(size_t i0, size_t j0)
            {
                return getProxy<RegionProxy<h_, w_>>(i0, j0);
            }

            const ColumnProxy&	col(size_t _i) const { return getProxy<ColumnProxy>(0, _i); }
            ColumnProxy&		col(size_t _i) { return getProxy<ColumnProxy>(0, _i); }
            const RowProxy&		row(size_t _i) const { return getProxy<RowProxy>(_i, 0); }
            RowProxy&			row(size_t _i) { return getProxy<RowProxy>(_i, 0); }

            // Matrix operations
            Element			trace() const;
            const TransposeView&	transpose() const { return *reinterpret_cast<const TransposeView*>(this); }
            MatrixBase		inverse() const;

            // Component wise operations
            template<typename OtherM_>
            Derived cwiseProduct(const OtherM_& _b) const { return cwiseBinaryOperator(*this, _b, [](Element& dst, Element a, Element b) {dst = a * b; }); }
            template<typename OtherM_>
            Derived cwiseMax(const OtherM_& _b) const { return cwiseBinaryOperator(*this, _b, [](Element& dst, Element a, Element b) {dst = std::max(a, b); }); }
            template<typename OtherM_>
            Derived cwiseMin(const OtherM_& _b) const { return cwiseBinaryOperator(*this, _b, [](Element& dst, Element a, Element b) {dst = std::min(a, b); }); }

            Derived abs() const { return cwiseUnaryOperator(*this, [](Element& dst, Element x) { dst = std::abs(x); }); }
            Derived operator-	() const { return cwiseUnaryOperator(*this, [](Element& dst, Element x) { dst = -x; }); }

            // Basic modifiers
            void setIdentity();
            void setConstant(Element);
            void setZero() { setConstant(Element(0)); }
            void setOnes() { setConstant(Element(1)); }

        private:
            template<typename Proxy>
            Proxy& getProxy(size_t i, size_t j)
            {
                return *reinterpret_cast<Proxy*>(&(*this)(i, j));
            }

            template<typename Proxy>
            const Proxy& getProxy(size_t i, size_t j) const
            {
                return *reinterpret_cast<const Proxy*>(
                    &(*this)(i, j)
                    );
            }
        };

        //------------------------------------------------------------------------------------------------------------------
        template<size_t rows_, size_t cols_, typename Storage_>
        void MatrixBase<rows_, cols_, Storage_>::setIdentity()
        {
            for (size_t j = 0; j < cols_; ++j)
                for (size_t i = 0; i < rows_; ++i)
                    (*this)(i, j) = typename Storage_::Element(i == j ? 1 : 0);
        }

        //------------------------------------------------------------------------------------------------------------------
        // Visitor expressions
        //------------------------------------------------------------------------------------------------------------------
        template<typename Matrix_, typename Operator_>
        void cwiseVisitor(
            Matrix_& _m,
            const Operator_& _operation
        )
        {
            for (size_t j = 0; j < Matrix_::cols; ++j)
                for (size_t i = 0; i < Matrix_::rows; ++i)
                    _operation(_m(i, j));
        }

        //------------------------------------------------------------------------------------------------------------------
        template<typename Matrix_, typename Operator_, bool col_major_ = Matrix_::is_col_major>
        auto cwiseUnaryOperator(
            Matrix_& _m,
            const Operator_& _operation
        ) -> typename Matrix_::Derived
        {
            typename Matrix_::Derived result;
            for (size_t j = 0; j < Matrix_::cols; ++j)
                for (size_t i = 0; i < Matrix_::rows; ++i)
                    _operation(result(i, j), _m(i, j));
            return result;
        }

        //------------------------------------------------------------------------------------------------------------------
        template<typename MatrixA_, typename MatrixB_, typename Operator_>
        auto cwiseBinaryOperator(
            MatrixA_& _a,
            MatrixB_& _b,
            const Operator_& _operation
        ) -> typename MatrixA_::Derived
        {
            static_assert(
                MatrixA_::cols == MatrixB_::cols &&
                MatrixA_::rows == MatrixB_::rows,
                "Matrix sizes must be equal");
            typename MatrixA_::Derived result;
            for (size_t j = 0; j < MatrixA_::cols; ++j)
                for (size_t i = 0; i < MatrixA_::rows; ++i)
                    _operation(result(i, j), _a(i, j), _b(i, j));
            return result;
        }

        //------------------------------------------------------------------------------------------------------------------
        template<size_t rows_, size_t cols_, typename Storage_>
        void MatrixBase<rows_, cols_, Storage_>::setConstant(typename Storage_::Element _c)
        {
            cwiseVisitor(*this, [=](typename Storage_::Element& _a) {_a = _c; });
        }

        //------------------------------------------------------------------------------------------------------------------
        template<typename MatA_, typename MatB_>
        auto operator+(
            const MatA_& _a,
            const MatB_& _b
            ) -> typename MatA_::Derived
        {
            using T = typename MatA_::Element;
            return cwiseBinaryOperator(_a, _b, [](T& res, T a, T b) { res = a + b; });
        }

        //------------------------------------------------------------------------------------------------------------------
        template<typename MatA_, typename MatB_>
        auto operator-(
            const MatA_& _a,
            const MatB_& _b
            ) -> typename MatA_::Derived
        {
            using T = typename MatA_::Element;
            return cwiseBinaryOperator(_a, _b, [](T& res, T a, T b) { res = a - b; });
        }

        //------------------------------------------------------------------------------------------------------------------
        template<size_t m_, size_t n_, typename S_>
        auto operator*(
            const MatrixBase<m_, n_, S_>& _a,
            typename S_::Element _k
            ) -> typename S_::Derived
        {
            using T = typename S_::Element;
            return cwiseUnaryOperator(_a, [=](T& res, T a) { res = a * _k; });
        }

        //------------------------------------------------------------------------------------------------------------------
        template<size_t m_, size_t n_, typename S_>
        auto operator*(
            typename S_::Element _k,
            const MatrixBase<m_, n_, S_>& _a
            ) -> typename S_::Derived
        {
            return _a * _k;
        }

        //------------------------------------------------------------------------------------------------------------------
        template<typename MatA_, typename MatB_>
        auto min(
            const MatA_& _a,
            const MatB_& _b
        ) -> typename MatA_::Derived
        {
            using T = typename MatA_::Element;
            return cwiseBinaryOperator(_a, _b, [](T& res, T a, T b) { res = math::min(a, b); });
        }

        //------------------------------------------------------------------------------------------------------------------
        template<typename MatA_, typename MatB_>
        auto max(
            const MatA_& _a,
            const MatB_& _b
        ) -> typename MatA_::Derived
        {
            using T = typename MatA_::Element;
            return cwiseBinaryOperator(_a, _b, [](T& res, T a, T b) { res = math::max(a, b); });
        }

    }	// namespace math
}	// namespace rev
