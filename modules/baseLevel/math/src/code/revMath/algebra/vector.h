//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Linear algebra's vector classes

#ifndef _REV_CORE_MATH_ALGEBRA_VECTOR_H_
#define _REV_CORE_MATH_ALGEBRA_VECTOR_H_

#include <cmath>
#include <revMath/numericTraits.h>

namespace rev { namespace math
{
	//------------------------------------------------------------------------------------------------------------------
	template<class Number_>
	class Vector2
	{
	public:
		// Copy, construction and destruction
		Vector2() {}											// Default constructor
		//Vector2(const Vector2<Number_>& _v);					// Copy constructor
		explicit Vector2(const Number_);						// Single value constructor
		Vector2(const Number_ _x, const Number_ _y);			// Data construction
		Vector2<Number_>& operator=(const Vector2<Number_>&);	// Assignement operator

		// Equality and inequality
		bool	operator==	(const Vector2<Number_>&) const;
		bool	operator!=	(const Vector2<Number_>&) const;

		// Adition and substraction
		Vector2<Number_>	operator+	(const Vector2<Number_>&) const;	// Adition
		Vector2<Number_>&	operator+=	(const Vector2<Number_>&);			// Adition
		Vector2<Number_>	operator-	(const Vector2<Number_>&) const;	// Substraction
		Vector2<Number_>&	operator-=	(const Vector2<Number_>&);			// Substraction
		Vector2<Number_>	operator-	() const;				// Return the oposite of this vector

		// Products
		Vector2<Number_>	operator*	(const Number_) const;				// Product by scalar
		Vector2<Number_>&	operator*=	(const Number_);					// Product by scalar
		Number_				operator*	(const Vector2<Number_>&) const;	// Dot product

		// Matrix-like accessors
				Number_& operator[] (unsigned int _index)		{ return (reinterpret_cast<Number_*>(this))[_index];		}
		const	Number_& operator[] (unsigned int _index) const { return (reinterpret_cast<const Number_*>(this))[_index];	}

		// Common constants
#ifdef _WIN32
		static			 Vector2<Number_>	zero();
#else // !WIN32
		static constexpr Vector2<Number_>	zero();
#endif // !WIN32

	public:
		// Actual data
		Number_ x, y;
	};

	//------------------------------------------------------------------------------------------------------------------
	template<class Number_>
	class Vector3
	{
	public:
		Vector3() {}
		explicit Vector3(const Number_ _v):x(_v),y(_v),z(_v) {}
		Vector3(const Vector2<Number_>& _v, Number_ _z):x(_v.x),y(_v.y),z(_z) {}
		Vector3(Number_ _x, Number_ _y, Number_ _z):x(_x),y(_y),z(_z){}
		Vector3<Number_>& operator=(const Vector3<Number_>&);

		// Equality and inequality
		bool	operator==	(const Vector3<Number_>&) const;
		bool	operator!=	(const Vector3<Number_>&) const;

		// Adition and substraction
		Vector3<Number_>	operator+	(const Vector3<Number_>&) const;	// Adition
		Vector3<Number_>&	operator+=	(const Vector3<Number_>&);			// Adition
		Vector3<Number_>	operator-	(const Vector3<Number_>&) const;	// Substraction
		Vector3<Number_>&	operator-=	(const Vector3<Number_>&);			// Substraction
		Vector3<Number_>	operator-	() const;				// Return the oposite of this vector

		// Products
		Vector3<Number_>	operator*	(const Number_) const;				// Product by scalar
		Vector3<Number_>&	operator*=	(const Number_);					// Product by scalar
		Vector3<Number_>	operator/	(const Number_) const;				// Division by scalar
		Vector3<Number_>&	operator/=	(const Number_);					// Division by scalar
		Number_				operator*	(const Vector3<Number_>&) const;	// Dot product
		Vector3<Number_>	operator^	(const Vector3<Number_>&) const;	// Cross product

		// Vector norm and normalization
		Number_		norm		() const;					///< Vector norm
		Number_		sqNorm		() const;					///< Vector square norm
		Vector3		normalized	() const;					///< Returns a normalized version of the vector
		Vector3&	normalize	();							///< Normalizes the vector

		// Common vectors
		static Vector3 zero () { return Vector3(NumericTraits<Number_>::zero()); }
		static Vector3 xAxis() { return Vector3(NumericTraits<Number_>::one(), NumericTraits<Number_>::zero(), NumericTraits<Number_>::zero()); }
		static Vector3 yAxis() { return Vector3(NumericTraits<Number_>::zero(), NumericTraits<Number_>::one(), NumericTraits<Number_>::zero()); }
		static Vector3 zAxis() { return Vector3(NumericTraits<Number_>::zero(), NumericTraits<Number_>::zero(), NumericTraits<Number_>::one()); }

				Number_& operator[] (unsigned int _index)		{ return (reinterpret_cast<Number_*>(this))[_index];		}
		const	Number_& operator[] (unsigned int _index) const { return (reinterpret_cast<const Number_*>(this))[_index];	}
	public:
		Number_ x, y, z;
	};

	//------------------------------------------------------------------------------------------------------------------
	template<class Number_>
	class Vector4
	{
	public:
		Vector4() {}
		Vector4(Number_ _x, Number_ _y, Number_ _z, Number_ _w):x(_x),y(_y),z(_z),w(_w){}

				Number_& operator[] (unsigned int _index)		{ return (reinterpret_cast<Number_*>(this))[_index];		}
		const	Number_& operator[] (unsigned int _index) const { return (reinterpret_cast<const Number_*>(this))[_index];	}
	public:
		Number_ x, y, z, w;
	};

	//------------------------------------------------------------------------------------------------------------------
	// Useful predefined types
	typedef Vector2<float>		Vec2f;	// 2d float vector
	typedef Vector2<int>		Vec2i;	// 2d signed integer vector
	typedef Vector2<unsigned>	Vec2u;	// 2d unsigned integer vector
	typedef Vector3<float>		Vec3f;	// 3d float vector
	typedef Vector3<int>		Vec3i;	// 3d signed integer vector
	typedef Vector3<unsigned>	Vec3u;	// 3d unsigned integer vector

	//------------------------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------------------------
	template<typename N_>
	inline Vector2<N_>::Vector2(const N_ _x)
		:x(_x),y(_x) {}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector2<N_>::Vector2(const N_ _x, const N_ _y)
		:x(_x),y(_y) {}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector2<N_>& Vector2<N_>::operator=(const Vector2<N_>& _v)
	{
		x=_v.x;
		y=_v.y;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline bool Vector2<N_>::operator==(const Vector2<N_>& _v) const
	{
		return (x == _v.x) && (y == _v.y);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline bool Vector2<N_>::operator!=(const Vector2<N_>& _v) const
	{
		return (x != _v.x) || (y != _v.y);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector2<N_>	Vector2<N_>::operator+ (const Vector2<N_>& _v) const
	{
		return Vector2<N_>(x+_v.x, y+_v.y);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector2<N_>& Vector2<N_>::operator+= (const Vector2<N_>& _v)
	{
		x += _v.x;
		y += _v.y;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector2<N_>	Vector2<N_>::operator- (const Vector2<N_>& _v) const
	{
		return Vector2<N_>(x-_v.x, y-_v.y);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector2<N_>& Vector2<N_>::operator-= (const Vector2<N_>& _v)
	{
		x -= _v.x;
		y -= _v.y;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector2<N_> Vector2<N_>::operator- () const
	{
		return Vector2<N_>(-x, -y);
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector2<N_> Vector2<N_>::operator*(const N_ _k) const
	{
		return Vector2<N_>(x * _k, y * _k);
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector2<N_>& Vector2<N_>::operator*=(const N_ _k)
	{
		x*=_k;
		y*=_k;
		return *this;
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline N_ Vector2<N_>::operator*(const Vector2<N_>& _b) const
	{
		return x * _b.x + y * _b.y;
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	#ifdef _WIN32
	inline Vector2<N_> Vector2<N_>::zero()
	#else // !_WIN32
	inline Vector2<N_> constexpr Vector2<N_>::zero()
	#endif // !_WIN32
	{
		return Vector2<N_>(NumericTraits<N_>::zero(), NumericTraits<N_>::zero());
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_>& Vector3<N_>::operator=(const Vector3<N_>& _v)
	{
		x=_v.x;
		y=_v.y;
		z=_v.z;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline bool Vector3<N_>::operator==(const Vector3<N_>& _v) const
	{
		return (x == _v.x) && (y == _v.y) && (z == _v.z);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline bool Vector3<N_>::operator!=(const Vector3<N_>& _v) const
	{
		return (x != _v.x) || (y != _v.y) || (z != _v.z);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_>	Vector3<N_>::operator+ (const Vector3<N_>& _v) const
	{
		return Vector3<N_>(x+_v.x, y+_v.y, z+_v.z);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_>& Vector3<N_>::operator+= (const Vector3<N_>& _v)
	{
		x += _v.x;
		y += _v.y;
		z += _v.z;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_>	Vector3<N_>::operator- (const Vector3<N_>& _v) const
	{
		return Vector3<N_>(x-_v.x, y-_v.y, z-_v.z);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_>& Vector3<N_>::operator-= (const Vector3<N_>& _v)
	{
		x -= _v.x;
		y -= _v.y;
		z -= _v.z;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_> Vector3<N_>::operator- () const
	{
		return Vector3<N_>(-x, -y, -z);
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_> Vector3<N_>::operator*(const N_ _k) const
	{
		return Vector3<N_>(x * _k, y * _k, z * _k);
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_>& Vector3<N_>::operator*=(const N_ _k)
	{
		x*=_k;
		y*=_k;
		z*=_k;
		return *this;
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_> Vector3<N_>::operator/(const N_ _k) const
	{
		N_ inv = 1.f / _k;
		return Vector3<N_>(x * inv, y * inv, z * inv);
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_>& Vector3<N_>::operator/=(const N_ _k)
	{
		N_ inv = 1.f / _k;
		x*=inv;
		y*=inv;
		z*=inv;
		return *this;
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline N_ Vector3<N_>::operator*(const Vector3<N_>& _b) const
	{
		return x * _b.x + y * _b.y + z * _b.z;
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_> Vector3<N_>::operator^(const Vector3<N_>& _b) const
	{
		return Vector3<N_>(	y*_b.z - z*_b.y,
							z*_b.x - x*_b.z,
							x*_b.y - y*_b.x);
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline N_ Vector3<N_>::norm	() const
	{
		return sqrt(x*x+y*y+z*z);
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline N_ Vector3<N_>::sqNorm() const
	{
		return x*x+y*y+z*z;
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_> Vector3<N_>::normalized() const
	{
		N_ factor = 1.f / norm();
		return Vector3<N_>(x*factor, y*factor, z*factor);
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	inline Vector3<N_>& Vector3<N_>::normalize()
	{
		N_ factor = 1.f / norm();
		x*=factor;
		y*=factor;
		z*=factor;
		return *this;
	}

}	// namespace math
}	// namespace rev

#endif // _REV_CORE_MATH_ALGEBRA_VECTOR_H_
