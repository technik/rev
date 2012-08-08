//----------------------------------------------------------------------------------------------------------------------
// Rose: Robot oriented simulation environment
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On August 8th, 2012
//----------------------------------------------------------------------------------------------------------------------
// 2d ackerman vehicle agent

#include "ackerman2dVehicle.h"

using namespace rev::math;

namespace rose { namespace agent
{
	//------------------------------------------------------------------------------------------------------------------
	Ackerman2dVehicle::Ackerman2dVehicle()
		:mPosition(Vec2f::zero())
		,mVelocity(Vec2f::zero())
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	void Ackerman2dVehicle::simulate(unsigned _ms)
	{
		float time = _ms * 0.001f;
		mPosition += mVelocity * time;
	}

}	// namespace agent
}	// namespace rose