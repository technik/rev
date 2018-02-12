//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#pragma once
#include <vector>
#include <math/algebra/vector.h>
#include <memory>

namespace rev { namespace graphics {

	class Material;
	class RenderGeom;
	class RenderObj;

	class RenderScene
	{
	public:
		RenderScene()
		{
			mLightClr = math::Vec3f::ones();
		}

		// Accessors
		const math::Vec3f& lightClr() const { return mLightClr; }
		std::vector<std::weak_ptr<RenderObj>>& renderables()				{ return mRenderables; }
		const std::vector<std::weak_ptr<RenderObj>>& renderables() const	{ return mRenderables; }

	private:
		math::Vec3f									mLightClr;
		std::vector<std::weak_ptr<RenderObj>>		mRenderables;
	};

}}	// namespace rev::graphics