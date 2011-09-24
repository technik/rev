////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, code tools, log
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on August 21st, 2011
////////////////////////////////////////////////////////////////////////////////
// log

#ifndef _REV_REVCORE_CODETOOLS_LOG_LOG_H_
#define _REV_REVCORE_CODETOOLS_LOG_LOG_H_

#if defined(_DEBUG)
#include <iostream>
#endif // _DEBUG


#if defined(ANDROID) && defined(_DEBUG)
#include <android/log.h>
#define LOG_ANDROID(...) __android_log_print(ANDROID_LOG_INFO, "REV_LOG", __VA_ARGS__)
#else // !(ANDROID && _DEBUG)
#define LOG_ANDROID(...) (__VA_ARGS__)
#endif // ANDROID && _DEBUG

namespace rev	{	namespace codeTools
{
	class SLog
	{
	public:
		// ---- Singleton lifecycle --------------------------------------------
		static	void	init();			///< Creates and inits the system.
		static	void	end	();			///< Finishes and deletes the system.

		template< typename _T>
		static	void	log	(_T _msg);	///< logs _msg.

	private:
		//----------------------------------------------------------------------
		// Private constructor and destructor for the singleton
		//----------------------------------------------------------------------
		SLog		();			///< Constructor.
		~SLog		();			///< Destructor.
	private:
		static	SLog* s_pLog;	///< Singleton instance.
	};
#ifdef _DEBUG
	//--------------------------------------------------------------------------
	template<typename _T>
	void SLog::log(_T _msg)
	{
		std::cout << _msg << std::endl;
	}
#else
	//--------------------------------------------------------------------------
	template<typename _T>
	void SLog::log(_T /*_msg*/)
	{
		// Intentionally blank
	}
#endif

} // namespace codeTools
} // namespace rev

#endif // _REV_REVCORE_CODETOOLS_LOG_LOG_H_
