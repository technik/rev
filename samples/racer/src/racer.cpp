//--------------------------------------------------------------------------------------------------
// Revolution Engine
//--------------------------------------------------------------------------------------------------
// Copyright 2021 Carmelo J Fdez-Aguera
// 
// Permission is hereby granted, free of charge, to any person obtaining a copy of this software
// and associated documentation files (the "Software"), to deal in the Software without restriction,
// including without limitation the rights to use, copy, modify, merge, publish, distribute,
// sublicense, and/or sell copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in all copies or
// substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
// NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
// NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
// DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
#include <cassert>
#include <thread>

#include "racer.h"
#include <gfx/backend/Context.h>
#include <gfx/backend/commandQueue.h>

namespace rev
{
	//------------------------------------------------------------------------------------------------------------------
	void Racer::CommandLineOptions::registerOptions(core::CmdLineParser& args)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void Racer::getCommandLineOptions(core::CmdLineParser& args)
	{
		m_options.registerOptions(args);
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Racer::init()
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Racer::end()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void Racer::onResize()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Racer::updateLogic(TimeDelta dt)
	{
		return true;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Racer::render(TimeDelta dt)
	{
		auto& gfxQueue = gfx::RenderContext().GfxQueue();
		//auto& cmdLst = gfxQueue.getCommandList();

		// Get the back buffer
		//cmdLst.resourceBarrier();

		// Clear
		
		//gfxQueue.submitCommandList(cmdLst);

		// Present
	}

}	// namespace rev