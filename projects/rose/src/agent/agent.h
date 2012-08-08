//----------------------------------------------------------------------------------------------------------------------
// Rose: Robot oriented simulation environment
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// Agent interface

#ifndef _ROSE_AGENT_AGENT_H_
#define _ROSE_AGENT_AGENT_H_

namespace rose { namespace agent
{
	class Agent
	{
	public:
		virtual ~Agent() = 0; // Pure virtual destructor for proper agent destruction
	};

}	// namespace agent
}	// namespace rose

#endif // _ROSE_AGENT_AGENT_H_
