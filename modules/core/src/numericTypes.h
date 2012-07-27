//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On July 27th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Basic numeric types

#ifndef _REV_CORE_NUMERICTYPES_H_
#define _REV_CORE_NUMERICTYPES_H_

#include <climits>

typedef unsigned char	u8;
typedef signed char		s8;

#if UINT_MAX == 65535 // sizeof(int)==2
typedef unsigned			u16;
typedef signed				s16;
typedef unsigned long int	u32;
typedef signed long int		s32;
#else // sizeof(int)==4

typedef unsigned short	u16;
typedef signed short	s16;
typedef unsigned int	u32;
typedef signed int		s32;
#endif // sizeof(int)==4

namespace rev { typedef	float TReal; }

#endif // _REV_CORE_NUMERICTYPES_H_