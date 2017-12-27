//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/03
//----------------------------------------------------------------------------------------------------------------------
// Generic mathematical vector
#ifndef _REV_MATH_ALGEBRA_VECTOR_H_
#define _REV_MATH_ALGEBRA_VECTOR_H_

#include <cassert>

namespace rev {
	namespace math {

		template<typename T, unsigned dim, typename Derived>
		class VectorBase {
		public:
			inline T		operator[](unsigned i)			{ return m[i]; }
			inline const T&	operator[](unsigned i) const	{ return m[i]; }

			Derived& operator+=(const Derived& b);
			Derived& operator-=(const Derived& b);

			Derived& operator*=(const T k);
			Derived& operator/=(const T k);
		protected:
			T m[dim];
		};

		//--------------------------------------------------------------------------------------------------------------
		// Base vector operations
		//--------------------------------------------------------------------------------------------------------------
		template<typename T, unsigned dim, typename Derived>
		Derived& VectorBase<T, dim, Derived>::operator+=(const Derived& b) {
			for(unsigned i = 0; i < dim; ++i) {
				m[i] += b.m[i];
			}
			return *static_cast<Derived*>(this);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T, unsigned dim, typename Derived>
		Derived& VectorBase<T, dim, Derived>::operator-=(const Derived& b) {
			for(unsigned i = 0; i < dim; ++i) {
				m[i] -= b.m[i];
			}
			return *static_cast<Derived*>(this);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T, unsigned dim, typename Derived>
		Derived& VectorBase<T, dim, Derived>::operator*=(const T k) {
			for(unsigned i = 0; i < dim; ++i) {
				m[i] *= k;
			}
			return *static_cast<Derived*>(this);
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T, unsigned dim, typename Derived>
		Derived& VectorBase<T, dim, Derived>::operator/=(const T k) {
			for(unsigned i = 0; i < dim; ++i) {
				m[i] /= k;
			}
			return *static_cast<Derived*>(this);
		}

		//--------------------------------------------------------------------------------------------------------------
		// Specializations
		//--------------------------------------------------------------------------------------------------------------
		// Generic dimension vector
		template<typename T, unsigned dim>
		class Vector : public VectorBase<T,dim,Vector<T,dim>> {};

		// Vector 2 specialization
		template<typename T>
		class Vector2 : public VectorBase<T, 2, Vector2<T>> {
		public:
			float&	x()			{ return m[0]; }
			float	x() const	{ return m[0]; }
			float&	y()			{ return m[1]; }
			float	y() const	{ return m[1]; }
		};

		// Vector 3 specialization
		template<typename T>
		class Vector3 : public VectorBase<T, 3, Vector3<T>> {
		public:
			float&	x()			{ return m[0]; }
			float	x() const	{ return m[0]; }
			float&	y()			{ return m[1]; }
			float	y() const	{ return m[1]; }
			float&	z()			{ return m[2]; }
			float	z() const	{ return m[2]; }
		};

		// Vector 4 specialization
		template<typename T>
		class Vector4 : public VectorBase<T, 4, Vector4<T>> {
		public:
			float&	x()			{ return m[0]; }
			float	x() const	{ return m[0]; }
			float&	y()			{ return m[1]; }
			float	y() const	{ return m[1]; }
			float&	z()			{ return m[2]; }
			float	z() const	{ return m[2]; }
			float&	w()			{ return m[3]; }
			float	w() const	{ return m[3]; }
		};

		//--------------------------------------------------------------------------------------------------------------
		// Generic vector operations
		//--------------------------------------------------------------------------------------------------------------
		template<typename T, unsigned dim>
		Vector<T,dim> operator+(const Vector<T, dim>& a, const Vector<T, dim>& b) {
			Vector<T,dim> result;
			for(unsigned i = 0; i < dim; ++i)
				result[i] = a[i]+b[i];
			return result;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T, unsigned dim>
		Vector<T,dim> operator-(const Vector<T, dim>& a, const Vector<T, dim>& b) {
			Vector<T,dim> result;
			for(unsigned i = 0; i < dim; ++i)
				result[i] = a[i]-b[i];
			return result;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T, unsigned dim>
		Vector<T,dim> operator*(const Vector<T, dim>& x, T k) {
			Vector<T,dim> result;
			for(unsigned i = 0; i < dim; ++i)
				result[i] = x[i]*k;
			return result;
		}

		//--------------------------------------------------------------------------------------------------------------
		template<typename T, unsigned dim>
		Vector<T,dim> operator/(const Vector<T, dim>& x, T k) {
			Vector<T,dim> result;
			for(unsigned i = 0; i < dim; ++i)
				result[i] = x[i]/k;
			return result;
		}

		// 2D vector
		template<typename T_>
		class Vector<T_, 2> {
		public:
			T_ x, y;
		public:
			// Construction
			Vector() = default;
			Vector(const Vector&) = default;
			Vector(const T_& _r) : x(_r), y(_r) {}
			Vector(const T_& _x, const T_& _y) : x(_x), y(_y) {}

			// Adition and substraction
			Vector	operator-	() const;				// Return the oposite of this vector

			// Products
			Vector&	operator*=	(const T_);				// Product by scalar
			Vector&	operator/=	(const T_);				// Division by scalar
			T_		operator*	(const Vector&) const;	// Dot product
			T_		operator^	(const Vector&) const;	// Cross product

			// Vector norm and normalization
			T_		norm() const;						//< Vector norm
			T_		sqNorm() const;						///< Vector square norm
			Vector	normalized() const;					///< Returns a normalized version of the vector
			Vector&	normalize();						///< Normalizes the vector
		};

		// 3D vector
		template<typename T_>
		class Vector<T_, 3> {
		public:
			T_ x, y, z;
		public:
			// Construction
			Vector() = default;
			Vector(const Vector&) = default;
			Vector(const Vector<T_,2>& _v, T_ _z) :x(_v.x), y(_v.y), z(_z) {}
			explicit Vector(const T_ _v) :x(_v), y(_v), z(_v) {}
			Vector(T_ _x, T_ _y, T_ _z) : x(_x), y(_y), z(_z) {}

			// Accessors
			T_&			operator[] (unsigned _index)		{ return (reinterpret_cast<T_*>(this))[_index]; }
			const T_&	operator[] (unsigned _index) const	{ return (reinterpret_cast<const T_*>(this))[_index]; }

			// Adition and substraction
			Vector	operator-	() const;				// Return the oposite of this vector

			// Products
			Vector&	operator*=	(const T_);				// Product by scalar
			Vector&	operator/=	(const T_);				// Division by scalar
			T_		operator*	(const Vector&) const;	// Dot product
			Vector	operator^	(const Vector&) const;	// Cross product

			// Vector norm and normalization
			T_		norm() const;						//< Vector norm
			T_		sqNorm() const;						///< Vector square norm
			Vector	normalized() const;					///< Returns a normalized version of the vector
			Vector&	normalize();						///< Normalizes the vector

			// Common vectors
			static Vector zero() { return Vector(0.f, 0.f, 0.f); }
			static Vector xAxis() { return Vector(T_(1.f), T_(0.f), T_(0.f)); }
			static Vector yAxis() { return Vector(T_(0.f), T_(1.f), T_(0.f)); }
			static Vector zAxis() { return Vector(T_(0.f), T_(0.f), T_(1.f)); }

		};

		// External operators
		template<class T_>
		T_	max(const T_& _a, const T_& _b);
		
		template<class T_>
		T_	min(const T_& _a, const T_& _b);


		typedef Vector<unsigned, 2> Vec2u;
		typedef Vector<int, 2>		Vec2i;
		typedef Vector<float, 2>	Vec2f;
		typedef Vector<double, 2>	Vec2d;

		typedef Vector3<unsigned>	Vec3u;
		typedef Vector3<int>		Vec3i;
		typedef Vector3<float>		Vec3f;
		typedef Vector3<double>		Vec3d;

		typedef Vector4<unsigned>	Vec4u;
		typedef Vector4<int>		Vec4i;
		typedef Vector4<float>		Vec4f;
		typedef Vector4<double>		Vec4d;
		
	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_ALGEBRA_VECTOR_H_