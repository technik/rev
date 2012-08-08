//----------------------------------------------------------------------------------------------------------------------
// Rose: Robot oriented simulation environment
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// 2d ackerman vehicle agent

#ifndef _ROSE_AGENT_VEHICLE_2D_ACKERMAN_ACKERMAN2DVEHICLE_H_
#define _ROSE_AGENT_VEHICLE_2D_ACKERMAN_ACKERMAN2DVEHICLE_H_

#include <agent/agent.h>

namespace rose { namespace agent
{
	class Ackerman2dVechicle : Agent
	{
	public:
		// Destructor
		~Ackerman2dVehicle() {}

		void simulate(unsigned _milliseconds);

	private:
	};

}	// namespace agent
}	// namespace rose

#endif // _ROSE_AGENT_VEHICLE_2D_ACKERMAN_ACKERMAN2DVEHICLE_H_
