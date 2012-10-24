//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Linear algebra's vector classes

#ifndef _REV_CORE_MATH_ALGEBRA_VECTOR_H_
#define _REV_CORE_MATH_ALGEBRA_VECTOR_H_

#include <revCore/math/numericTraits.h>

namespace rev { namespace math
{
	//------------------------------------------------------------------------------------------------------------------
	template<class Number_>
	class Vector2
	{
	public:
		// Copy, construction and destruction
		Vector2() {}											// Default constructor
		Vector2(const Vector2<Number_>& _v);					// Copy constructor
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
		Vector2<Number_>	operator*	(const Vector2<Number_>&) const;	// Dot product
		Vector2<Number_>&	operator*=	(const Vector2<Number_>&);			// Dot product
		Vector2<Number_>	operator^	(const Vector2<Number_>&) const;	// Cross product
		Vector2<Number_>&	operator^=	(const Vector2<Number_>&);			// Cross product
		Vector2<Number_>	operator|	(const Vector2<Number_>&) const;	// Component wise product
		Vector2<Number_>&	operator|=	(const Vector2<Number_>&);			// Component wise product

		// Matrix-like accessors
		Number_&			operator()	(unsigned _index);
		const Number_&		operator()	(unsigned _index) const;

		// Common constants
#ifdef WIN32
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
	template<class N_>
	Vector2<N_>::Vector2(const Vector2<N_>& _v)
		:x(_v.x)
		,y(_v.y)
	{}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	Vector2<N_>::Vector2(const N_ _x, const N_ _y)
		:x(_x)
		,y(_y)
	{}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	Vector2<N_>& Vector2<N_>::operator=(const Vector2<N_>& _v)
	{
		x=_v.x;
		y=_v.y;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	bool Vector2<N_>::operator==(const Vector2<N_>& _v) const
	{
		return (x == _v.x) && (y == _v.y);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	bool Vector2<N_>::operator!=(const Vector2<N_>& _v) const
	{
		return (x != _v.x) || (y != _v.y);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	Vector2<N_>	Vector2<N_>::operator+ (const Vector2<N_>& _v) const
	{
		return Vector2<N_>(x+_v.x, y+_v.y);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	Vector2<N_>& Vector2<N_>::operator+= (const Vector2<N_>& _v)
	{
		x += _v.x;
		y += _v.y;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	Vector2<N_>	Vector2<N_>::operator- (const Vector2<N_>& _v) const
	{
		return Vector2<N_>(x-_v.x, y-_v.y);
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	Vector2<N_>& Vector2<N_>::operator-= (const Vector2<N_>& _v)
	{
		x -= _v.x;
		y -= _v.y;
		return *this;
	}

	//------------------------------------------------------------------------------------------------------------------
	template<class N_>
	Vector2<N_> Vector2<N_>::operator- () const
	{
		return Vector2<N_>(-x, -y);
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	Vector2<N_> Vector2<N_>::operator*(const N_ _k) const
	{
		return Vector2<N_>(x * _k, y * _k);
	}

	//----------------------------------------------------------------------------------------------------------------
	template<class N_>
	#ifdef WIN32
	Vector2<N_> Vector2<N_>::zero()
	#else // !WIN32
	Vector2<N_> constexpr Vector2<N_>::zero()
	#endif // !WIN32
	{
		return Vector2<N_>(NumericTraits<N_>::zero(), NumericTraits<N_>::zero());
	}

}	// namespace math
}	// namespace rev

#endif // _REV_CORE_MATH_ALGEBRA_VECTOR_H_
