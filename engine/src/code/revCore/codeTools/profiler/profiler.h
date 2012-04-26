////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, code tools, log
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on April 18th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System profiler

#ifndef _REV_CORE_CODETOOLS_PROFILER_PROFILER_H_
#define _REV_CORE_CODETOOLS_PROFILER_PROFILER_H_

#ifdef REV_PROFILER
#include <vector.h>
#include <rtl/map.h>
#include <revCore/string.h>
#endif // REV_PROFILER

namespace rev { namespace codeTools
{
#ifdef REV_PROFILER

	// Profiler system
	class SProfiler
	{
	public:
		// Singleton interface
		static void			init();
		static void			end	();
		static SProfiler*	get	() {return sInstance;}
	private:
		SProfiler();
		~SProfiler();
		static SProfiler*	sInstance;

	public:
		void addEventStart(const char * _name);
		void addEventFinish();
		void resetFrame();

	private:
#ifdef WIN32
		typedef unsigned timeMarkT;
#endif // WIN32
		static timeMarkT	getTimeMark	();
		static double		timeFromTimeMark(timeMarkT);
		void		logProfilingResults	();

		struct eventMark
		{
			~eventMark(){}
			timeMarkT	startTime;
			timeMarkT	discountTime; // Accumulated overhead due to the profiling system itself
			const char*	name;
		};

		rtl::vector<eventMark>	mFunctionStack;

		class measureT
		{
		public:
			char * name;
			unsigned indent;
			~measureT() {}
			bool operator < (const measureT& _x) const { return compareStringsOrder(name, _x.name) < 0; }
		};
		rtl::map<measureT, timeMarkT>	mMeasuredTimes;
		unsigned	mNFrames;
	};
	// Profiler agents
	// Add one of this to each function you want to profile
	class CProfileFunction
	{
	public:
		CProfileFunction	(const char * _identifier)	{ SProfiler::get()->addEventStart(_identifier); }
		~CProfileFunction	()							{ SProfiler::get()->addEventFinish();			}
	private:
		CProfileFunction			(const CProfileFunction&)	{}
		CProfileFunction& operator= (const CProfileFunction&)	{}
	};

#else // !REV_PROFILER
	class CProfileFunction
	{
	public:
		CProfileFunction(const char*) {}
		// Empty
	};
#endif // !REV_PROFILER
}	// namespace codeTools
}	// namespace rev

#endif // _REV_CORE_CODETOOLS_PROFILER_PROFILER_H_
