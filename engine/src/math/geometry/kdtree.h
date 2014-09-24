//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/Sep/23
//----------------------------------------------------------------------------------------------------------------------
// KdTree structure
#ifndef _REV_MATH_GEOMETRY_KDTREE_H_
#define _REV_MATH_GEOMETRY_KDTREE_H_

#include <math/algebra/vector.h>
#include <core/types/typeTraits.h>

namespace rev {
	namespace math {

		// --- Notes ---
		// - Voxel size (maximun) is given at run-time in the dataset. This defines the max
		// level of recursion in the tree
		// - All nodes have either 0 or 2^k children

		template<unsigned dim_,bool grid_>
		class Kdtree {
		public:
			typedef Select<int,float,grid_>	Coordinate;
			typedef Vector<Coordinate,dim_>	Vector;

		public:
			template<class BuildPolicy_, class DataSet_>
			Kdtree(const DataSet_&);

			bool ray		(Point _a, Point _b, float& _distance) const override;
			bool isFree		(Point) const override;
		};

	}	// namespace math
}	// namespace rev

#endif // _REV_MATH_GEOMETRY_KDTREE_H_