////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, types
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 22nd, 2011
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// basic types

#ifndef _REV_REVCORE_TYPES_H_
#define _REV_REVCORE_TYPES_H_

#if defined(_linux) || defined(ANDROID) || defined (WIN32)

typedef unsigned char	u8;
typedef signed char		s8;
typedef unsigned short	u16;
typedef signed short	s16;
typedef unsigned int	u32;
typedef signed int		s32;

#endif // _linux || ANDROID || WIN32

namespace rev
{

#if defined(_linux) || defined(ANDROID) || defined (WIN32)

typedef		float		TReal;

#endif // _linux || ANDROID || WIN32

}	// namespace rev

#endif // _REV_REVCORE_TYPES_H_
