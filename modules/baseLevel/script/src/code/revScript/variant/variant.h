//-------------------------------------------------------------------------
// revScript
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On January 18th, 2013
//-------------------------------------------------------------------------
// Variant: Dynamic type data holder

#ifndef _REV_SCRIPT_VARIANT_VARIANT_H_
#define _REV_SCRIPT_VARIANT_VARIANT_H_

namespace rev { namespace script
{
	class Variant
	{
	public:
		enum class Type
		{
			nill,
			integer,
			real,
			text,
			vector
		};
	public:
		// Constructors
		Variant				();	// Defaults to nill
		Variant				(int);
		Variant				(float);

		// Copy and assignment
		Variant				(const Variant&);
		Variant& operator=	(const Variant&);
		Variant& operator=	(int);
		Variant& operator=	(float);

		// Accessors
		void	setNill		();
		int		asInteger	() const; // If the stored data is not an int, the result is undefined
		float	asReal		() const; // If the stored data is not a float, the result is undefined
		Type	type		() const;

	private:
		union Data
		{
			int		mInteger;
			float	mReal;
		};

	private:
		Type	mDataType;
		Data	mData;
	};

	//------------------------------------------------------------------------------------------------
	// Inline implementations
	//------------------------------------------------------------------------------------------------
	inline Variant::Variant()
		:mDataType(Type::nill)
	{}

	//------------------------------------------------------------------------------------------------
	inline Variant::Variant(int _x)
		:mDataType(Type::integer)
	{
		mData.mInteger = _x;
	}

	//------------------------------------------------------------------------------------------------
	inline Variant::Variant(float _x)
		:mDataType(Type::real)
	{
		mData.mReal = _x;
	}

	//------------------------------------------------------------------------------------------------
	inline int Variant::asInteger() const
	{
		return mData.mInteger;
	}

	//------------------------------------------------------------------------------------------------
	inline float Variant::asReal() const
	{
		return mData.mReal;
	}

	//------------------------------------------------------------------------------------------------
	inline void Variant::setNill()
	{
		mDataType = Type::nill;
	}

	//------------------------------------------------------------------------------------------------
	inline Variant::Type Variant::type() const
	{
		return mDataType;
	}

}	// namespace script
}	// namespace rev

#endif // _REV_SCRIPT_VARIANT_VARIANT_H_