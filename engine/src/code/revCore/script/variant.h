////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Script variant

#ifndef _REV_CORE_SCRIPT_CVARIANT_H_
#define _REV_CORE_SCRIPT_CVARIANT_H_

namespace rev
{
	class CVariant
	{
	public:
		enum	EDataType
		{
			eNill = 0,
			eInteger,
			eReal,
			eBool,
			eList,
			eDictionary
		};
	public:
		// ---- Construction, copy and deletion -----
		// Basic constructors
		CVariant			();
		CVariant			(const CVariant&);

		// Typed constructors
		CVariant			(int);
		CVariant			(double);
		CVariant			(bool);

		// Assignment
		CVariant& operator=	(const CVariant&);
		CVariant& operator=	(int);
		CVariant& operator=	(double);
		CVariant& operator=	(bool);

		// Destruction
		~CVariant			() {}
		
		// Accessors
		int			asInt		() const	{ return mData.i; }
		double		asDouble	() const	{ return mData.d; }
		bool		asBool		() const	{ return mData.b; }

		EDataType	type		() const	{ return mType;	  }
	public:
		// Comparison
		bool	operator==	(const CVariant&) const;
		bool	operator==	(int) const;
		bool	operator==	(double) const;
		bool	operator==	(bool) const;
		bool	operator!=	(const CVariant&) const;
		bool	operator!=	(int) const;
		bool	operator!=	(double) const;
		bool	operator!=	(bool) const;

	private:
		union UDataT
		{
			int	i;		// Int
			double d;	// Real
			bool b;		// bool
		};

	private:
		EDataType	mType;
		UDataT		mData;
	};
}	// namespace rev

#endif // _REV_CORE_SCRIPT_CVARIANT_H_
