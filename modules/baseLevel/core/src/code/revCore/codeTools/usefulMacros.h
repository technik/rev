//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On July 26th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Useful macors

#ifndef _REV_CORE_CODETOOLS_USEFULMACROS_H_
#define _REV_CORE_CODETOOLS_USEFULMACROS_H_

//----------------------------------------------------------------------------------------------------------------------
// Class construction, copy and destruction
//----------------------------------------------------------------------------------------------------------------------
// When declared under private, this macro disables copy of the enclosing class
// Android doesn't support move syntax
#ifdef ANDROID
#define REV_DECLARE_COPY( type_ )	\
	type_(const type_&);\
	type_& operator=(const type_&);
#else // !ANDROID
#define REV_DECLARE_COPY( type_ )	\
	type_(const type_&);\
	type_(type_&&);\
	type_& operator=(const type_&);\
	type_& operator=(type_&&);
#endif // !ANDROID
	
#define REV_UNUSED_PARAM( _param )	\
	(void)_param

#endif // _REV_CORE_CODETOOLS_USEFULMACROS_H_
