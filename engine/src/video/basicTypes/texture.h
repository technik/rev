//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Textures
#ifndef _REV_VIDEO_BASICTYPES_TEXTURE_H_
#define _REV_VIDEO_BASICTYPES_TEXTURE_H_

#include <math/algebra/vector.h>

namespace rev {
	namespace video {
		class Texture {
		public:
			enum class EImageFormat {
				rgb,
				rgba,
				alpha,
				luminance,
				lumiAlpha
			};

			enum class EByteFormat {
				eUnsignedByte,
				eByte,
				eUnsignedShort,
				eShort,
				eUnsignedInt,
				eInt,
				eFloat
			};

			math::Vec2u size;
			void * data;
			EImageFormat imgFormat;
			EByteFormat byteFormat;
		};
	}
}

#endif // _REV_VIDEO_BASICTYPES_TEXTURE_H_
