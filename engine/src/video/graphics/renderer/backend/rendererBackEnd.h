//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
// Created by Carmelo J. Fdez-Agüera Tortosa (a.k.a. Technik)
// 2014/April/07
//----------------------------------------------------------------------------------------------------------------------
// Render system's back end, the layer right on top of the video driver
#ifndef _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_RENDERERBACKEND_H_
#define _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_RENDERERBACKEND_H_

#include <cstdint>


#include <game/geometry/procedural/basic.h>
#include <math/algebra/matrix.h>
#include <math/algebra/vector.h>
#include <video/graphics/driver/graphicsDriver.h>
#include <video/graphics/shader/shader.h>
#include <vector>

namespace rev {
	namespace video {

		
		class RendererBackEnd {
		public:
			struct DrawCall {
				uint32_t indexOffset;
				uint32_t nIndices;
				uint32_t vertexOffset;
				// Uniforms
				struct UnifromData {
					const void* data = nullptr;
					size_t size = 0;
				} uniformData;
#ifdef REV_USE_VULKAN
				// TODO: Use descriptor sets instead of push constants
				//VkDescriptorSet mDescriptorSet;
#endif
				// TODO: Support hardware instancing. Uniforms that must be specified per instance,
				// must be fed in a vertex buffer with per-instance input rate
				// uint32_t nInstances;
				// perInstanceUniformInfo
				// - More than one buffer? maybe with different frequencies? think stereo rendering + instancing
				// batchUniformInfo?
			};
			struct DrawBatch {
				std::vector<DrawCall>	draws;
#ifdef REV_USE_VULKAN
				vk::Buffer indexBuffer;
				vk::Buffer vertexBuffer;
#endif
			};

		public:
			void draw(const DrawBatch& _batch);

#ifdef REV_USE_VULKAN
			VkCommandBuffer	mCommandBuffer = VK_NULL_HANDLE;
			VkPipelineLayout mActivePipelineLayout = VK_NULL_HANDLE;
#endif

#ifdef OPENGL_45
		public:
			struct DrawInfo {
				Shader::Ptr		program;
				math::Vec3f		lightDir;
				math::Vec3f		lightClr;
				math::Vec3f		viewPos;
				math::Mat44f	wvp; // Model view projection

				template<typename Type_>
				using Uniform = std::pair<std::string, Type_>;

				std::vector<Uniform<math::Vec3f>>	vec3Uniforms;
				std::vector<Uniform<Texture*>>		texUniforms;
				std::vector<Uniform<float>>			floatUniforms;
			};

			struct DrawCall {
				/// Things line uniforms, and render config
				DrawInfo			renderStateInfo;
				StaticRenderMesh*	mesh;
			};

			RendererBackEnd(GraphicsDriver* _driver) : mDriver(_driver) {}
			void draw(const DrawCall&);
			void drawSkybox(const DrawInfo&);
			void flush();

		private:
			StaticRenderMesh* mSkyBox = nullptr;
			GraphicsDriver* mDriver = nullptr;
#endif // OPENGL_45
		};
	}
}

#endif // _REV_VIDEO_GRAPHICS_RENDERER_BACKEND_BACKENDRENDERER_H_