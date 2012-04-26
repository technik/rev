////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, code tools, log
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on April 18th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// System profiler

#ifdef REV_PROFILER

#ifdef WIN32
#include <Windows.h>
#endif // WIN32

#include "profiler.h"
#include <revCore/codeTools/log/log.h>

namespace rev { namespace codeTools
{
	//------------------------------------------------------------------------------------------------------------------
	SProfiler * SProfiler::sInstance = 0;

	//------------------------------------------------------------------------------------------------------------------
	void SProfiler::init()
	{
		sInstance = new SProfiler();
	}

	//------------------------------------------------------------------------------------------------------------------
	void SProfiler::end()
	{
		delete sInstance;
	}

	//------------------------------------------------------------------------------------------------------------------
	SProfiler::SProfiler()
		:mNFrames(0)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	SProfiler::~SProfiler()
	{
		// Log results
		logProfilingResults();
	}

	//------------------------------------------------------------------------------------------------------------------
	void SProfiler::addEventStart(const char* _id)
	{
		// First of all, get the time
		timeMarkT startTime = getTimeMark(); // The start time of THIS function
		// Create a whole time mark and fill it
		eventMark mark;
		mark.name = _id;
		// Store it into the stack, and save the discount time
		mFunctionStack.push_back(mark);
		eventMark& storedMark = mFunctionStack.back();
		storedMark.startTime = getTimeMark();	// The profiled function starts at the end of this constructor
		storedMark.discountTime = storedMark.startTime - startTime; // How long did it take to run THIS function
	}

	//------------------------------------------------------------------------------------------------------------------
	void SProfiler::addEventFinish()
	{
		timeMarkT startTime = getTimeMark(); // start time of THIS function, end of the profiled function
		eventMark& closingEvent = mFunctionStack.back();
		timeMarkT profiledTime = startTime - closingEvent.startTime;

		measureT newMeasure;
		newMeasure.name = new char[stringLength(closingEvent.name)];
		copyString(newMeasure.name, closingEvent.name);
		newMeasure.indent =  mFunctionStack.size()-1;
		if(mMeasuredTimes.find(newMeasure) == mMeasuredTimes.end()) // No existing key like this
		{
			mMeasuredTimes[newMeasure] = profiledTime;
		}
		else
		{
			mMeasuredTimes[newMeasure] += profiledTime;
			//delete[] newMeasure.name;
		}
		mFunctionStack.pop_back();
		if(!mFunctionStack.empty())
			mFunctionStack.back().discountTime += closingEvent.discountTime + getTimeMark() - startTime;
	}

	//------------------------------------------------------------------------------------------------------------------
	void SProfiler::resetFrame()
	{
		++mNFrames;
	}

	//------------------------------------------------------------------------------------------------------------------
	SProfiler::timeMarkT SProfiler::getTimeMark()
	{
	#if defined (WIN32)
		// Get current time
		LARGE_INTEGER largeTicks;
		QueryPerformanceCounter(&largeTicks);
		return largeTicks.LowPart;
	#endif // WIN32
	}

	//------------------------------------------------------------------------------------------------------------------
	double SProfiler::timeFromTimeMark(SProfiler::timeMarkT _mark)
	{
	#if defined (WIN32)
		// Convert time difference to seconds
		LARGE_INTEGER frequency;
		QueryPerformanceFrequency(&frequency);
		unsigned long freq = frequency.LowPart + (frequency.HighPart << sizeof(unsigned));
		return double(_mark)/double(freq);
	#endif // WIN32
	}

	//------------------------------------------------------------------------------------------------------------------
	void SProfiler::logProfilingResults()
	{
		SLog::get()->log("----- Profiling results -----\n", eProfiler);
		SLog::get()->log("-- Profiled along ", eProfiler);
		SLog::get()->log(double(mNFrames), eProfiler);
		SLog::get()->log(" frames\n", eProfiler);
		SLog::get()->log("Total\t\t", eProfiler);
		SLog::get()->log("Average\t\n", eProfiler);
		rtl::map<measureT, timeMarkT>::iterator i;
		double totalTime = 0.0;
		for(i = mMeasuredTimes.begin(); i != mMeasuredTimes.end(); ++i)
		{
			//unsigned indent = i->first.second;
			//for(unsigned t=0; t < indent; ++t)
			//	revLog("\t", eProfiler);	// Indent n times
			double markTime = timeFromTimeMark(i->second);
			totalTime += markTime;
			double perFrameTime = markTime/double(mNFrames);
			SLog::get()->log(markTime, eProfiler);
			SLog::get()->log(" ", eProfiler);
			SLog::get()->log(perFrameTime, eProfiler);
			SLog::get()->log("\t", eProfiler);
			SLog::get()->log(const_cast<const char*>(i->first.name), eProfiler);
			SLog::get()->log("\n", eProfiler);
		}
		SLog::get()->log("-- Total time: ", eProfiler);
		SLog::get()->log(totalTime, eProfiler);
		SLog::get()->log(" seconds\n", eProfiler);
		SLog::get()->log("-- Average time per frame: ", eProfiler);
		SLog::get()->log(totalTime/mNFrames, eProfiler);
		SLog::get()->log(" seconds\n", eProfiler);
		SLog::get()->log("----- End of Profiling -----\n", eProfiler);
	}

}	// namespace codeTools
}	// namespace rev

#endif // REV_PROFILER
