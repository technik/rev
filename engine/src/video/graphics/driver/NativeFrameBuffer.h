//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
//----------------------------------------------------------------------------------------------------------------------
#pragma once

#ifdef OPENGL_45
#include "openGL/NativeFrameBufferGL.h"
#endif
#ifdef REV_USE_VULKAN
#include "vulkan/NativeFrameBufferVulkan.h"
#endif

namespace rev { namespace video {
#ifdef OPENGL_45
	typedef NativeFrameBufferGL	NativeFrameBuffer;
#endif
#ifdef REV_USE_VULKAN
	typedef NativeFrameBufferVulkan NativeFrameBuffer;
#endif
}}
