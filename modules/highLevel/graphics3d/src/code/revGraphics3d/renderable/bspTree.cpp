//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/Aug/22
//----------------------------------------------------------------------------------------------------------------------
// Bsp tree

#include "bspTree.h"

namespace rev { namespace graphics3d {

	//------------------------------------------------------------------------------------------------------------------
	void BspTree::reserve(unsigned _nNodes)
	{
		mNodes.reserve(_nNodes);
	}

}	// namespace rev
}	// namespace video