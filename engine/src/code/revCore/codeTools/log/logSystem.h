////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, code tools, log
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on April 9th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Log system internals

#ifndef _REV_CORE_CODETOOLS_LOG_LOGSYSTEM_H_
#define _REV_CORE_CODETOOLS_LOG_LOGSYSTEM_H_

#include <utility.h>
#include <vector.h>

#include <revCore/string.h>

#include "log.h"

namespace rev { namespace codeTools
{
	class SLog
	{
	public:
		// Singleton life cycle
		static	void	init	();
		static	void	end		();
		static	SLog*	get		();

	public:
		// Common use interface
		template < class T >
		void	log		( T _msg, ELogChannel _out );
		void	newLine	(ELogChannel _out);

		// Extra management
		void	enableGlobalLog		();
		void	disableGlobalLog	();
		void	enableChannel		(ELogChannel _channel);
		void	disableChannel		(ELogChannel _channel);
		void	flush				();
		void	setBufferCapacity	(unsigned _capacity);

	private:
		typedef char*	messageT;	// Channel, message text
		
		bool					mEnableGlobal;
		bool					mEnableChannel[eMaxLogChannel];
		rtl::vector<messageT>	mBuffer;
		unsigned				mBufferCapacity;

	private:
		// Singleton instantiation
		SLog();
		~SLog();
		static SLog * sInstance;
	};
	

	//--------------------------------------------------------------------------------------------------------------
	template<class T>
	void SLog::log( T _msg, ELogChannel _channel )
	{
		if(mEnableGlobal && mEnableChannel[_channel])
		{
			mBuffer.push_back(makeString(_msg));
			if(mBuffer.size >= mBufferCapacity)
				flush();
		}
	}
}	// namespace codeTools
}	// namespace rev

#endif // _REV_CORE_CODETOOLS_LOG_LOGSYSTEM_H_
