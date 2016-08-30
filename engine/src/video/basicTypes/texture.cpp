//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
// Textures
#include <video/basicTypes/texture.h>
#include <freeImage/FreeImage.h>
#include <video/graphics/driver/graphicsDriver.h>

namespace rev {
	namespace video {
		//--------------------------------------------------------------------------------------------------------------
		Texture* Texture::load(const std::string& _fileName, GraphicsDriver* _driver){
			FIBITMAP * bitmap = FreeImage_Load(FIF_JPEG, _fileName.c_str(), JPEG_ACCURATE);
			if(!bitmap)
				return nullptr;
			auto imgType = FreeImage_GetImageType(bitmap);
			auto colorType = FreeImage_GetColorType(bitmap);
			if(imgType != FIT_BITMAP || colorType != FIC_RGB) {
				FreeImage_Unload(bitmap);
				return nullptr;
			}
			math::Vec2u size;
			size.x = FreeImage_GetWidth(bitmap);
			size.y = FreeImage_GetHeight(bitmap);
			size_t nPixels = size.x*size.y;
			uint8_t *pixelData = new uint8_t[4*nPixels];
			for (size_t i = 0; i < nPixels; ++i) {
				pixelData[4 * i] = ((uint8_t*)bitmap->data)[3*i];
				pixelData[4 * i+1] = ((uint8_t*)bitmap->data)[3 * i+2];
				pixelData[4 * i+2] = ((uint8_t*)bitmap->data)[3 * i+1];
				pixelData[4 * i+3] = 255;
			}
			
			return _driver->createTexture(size, EImageFormat::rgba, EByteFormat::eUnsignedByte, pixelData);
		}
	}
}