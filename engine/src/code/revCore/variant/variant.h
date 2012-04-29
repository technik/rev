////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution engine
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on March 30th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// variant

#ifndef _REV_CORE_VARIANT_CVARIANT_H_
#define _REV_CORE_VARIANT_CVARIANT_H_

#include <vector.h>
#include <revCore/math/vector.h>
#include <revCore/string.h>

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
			ePointer,
			eString,
			eList,
			eDictionary
		};
	public:
		// ---- Construction, copy and deletion -----
		// Basic constructors
		CVariant					();
		CVariant					(const CVariant&);

		// Typed constructors
		CVariant					(int);
		CVariant					(double);
		CVariant					(bool);
		CVariant					(const char * _str);
		CVariant					(const string& _str);
		CVariant					(void* _ptr);

		// Destruction
		~CVariant					() {}

		// Assignment
		CVariant&		operator=	(const CVariant&);
		CVariant&		operator=	(int);
		CVariant&		operator=	(double);
		CVariant&		operator=	(bool);
		CVariant&		operator=	(const char * _str);
		CVariant&		operator=	(const string& _str);
		CVariant&		operator=	(void * _ptr);
		void			setNill		();
		void			append		(const CVariant& _x);
		
		// Accessors
		int				asInt		() const;
		double			asDouble	() const;
		bool			asBool		() const;
		void*			asPointer	() const;
		const char*		asString	() const;
		CVec3			asVec3		() const;
		CVec2			asVec2		() const;
		CVariant&		operator[]	(unsigned _idx);
		const CVariant&	operator[]	(unsigned _idx) const;

		EDataType		type		() const	{ return mType;	  }

		// Log
		void			log			(unsigned _indent = 0);
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
			int		i;	// Int
			double	d;	// Real
			bool	b;	// Bool
			void*	p;	// Void pointer
		};

	private:
		EDataType				mType;
		UDataT					mData;
		rtl::vector<CVariant>	mList;
		string			mString;
	};
}	// namespace rev

#endif // _REV_CORE_VARIANT_CVARIANT_H_
