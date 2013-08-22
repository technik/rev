//----------------------------------------------------------------------------------------------------------------------
// Graphics 3d module
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// On 2013/Aug/22
//----------------------------------------------------------------------------------------------------------------------
// Bsp tree
#ifndef _REV_GRAPHICS3D_RENDERABLE_BSPTREE_H_
#define _REV_GRAPHICS3D_RENDERABLE_BSPTREE_H_

#include <revMath/algebra/vector.h>
#include <vector>

namespace rev { namespace graphics3d {

	class BspTree {
	public:
		void reserve(unsigned nNodes);

		struct Node {
			math::Vec3f normal;
			float		offset;
			int	positiveChildIdx;
			int	negativeChildIdx;
		};
		std::vector<Node>	mNodes;
	};

}	// namespace rev
}	// namespace video

#endif // _REV_GRAPHICS3D_RENDERABLE_BSPTREE_H_