//----------------------------------------------------------------------------------------------------------------------
// Revolution Engine
//----------------------------------------------------------------------------------------------------------------------
#include <cassert>
#include "player.h"
#include <math/algebra/vector.h>
#include <core/platform/fileSystem/file.h>
#include <core/time/time.h>
#include <core/tools/log.h>
#include <game/scene/meshRenderer.h>
#include <game/scene/transform/transform.h>
#include <graphics/debug/debugGUI.h>

using namespace rev::math;
using namespace rev::graphics;
using namespace rev::game;

namespace rev {

	const std::vector<Vec3f> vertices = {
		{1.f, 0.f, 1.f},
		{-1.f, 0.f, 1.f},
		{0.f,0.f, -1.f}
	};
	const std::vector<uint16_t> indices = { 0, 1, 2};

	//------------------------------------------------------------------------------------------------------------------
	bool Player::init(Window _window) {
		core::Time::init();

		assert(!mGfxDriver);
		mGfxDriver = GraphicsDriverGL::createDriver(_window);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LEQUAL);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
		if(mGfxDriver) {
			glClearColor(89.f/255.f,235.f/255.f,1.f,1.f);
			// Create shader
			core::File shaderFile("pbr.fx");
			mShader = Shader::createShader(shaderFile.bufferAsText());
			if(mShader)
				mShader->bind();
			// Camera
			mProjectionMtx = math::frustrumMatrix(0.8f, 4.f/3.f,1.0f,10000.f);
			// -- triangle --
			loadScene("sponza_crytek.scn");

			gui::init({1280, 720});
		}
		return mGfxDriver != nullptr;
	}

	struct MeshHeader
	{
		uint32_t nVertices;
		uint32_t nIndices;
	};

	struct RenderObjData {
		int meshIdx = -1;
		Mat44f transform;
	};

	//------------------------------------------------------------------------------------------------------------------
	void Player::loadScene(const char* _assetFileName)
	{
		core::File asset(_assetFileName);
		if(asset.sizeInBytes() == 0)
		{
			rev::core::Log::error("Unable to load asset");
			return;
		}
		auto ptr = asset.buffer();
		auto header = reinterpret_cast<const uint32_t*>(ptr);
		auto nMeshes = header[0];
		mMeshes.reserve(nMeshes);
		auto nObjects = header[1];
		ptr = &header[2];
		using VertexLine = RenderGeom::Vertex;
		std::vector<VertexLine>	vertexData;
		std::vector<uint16_t>	indices;
		for(size_t i = 0; i < nMeshes; ++i)
		{
			auto meshHeader = (const MeshHeader*)ptr;
			ptr = &meshHeader[1];
			vertexData.resize(meshHeader->nVertices);
			memcpy(vertexData.data(), ptr, vertexData.size()*sizeof(VertexLine));
			ptr = &reinterpret_cast<const VertexLine*>(ptr)[vertexData.size()];
			indices.resize(meshHeader->nIndices);
			memcpy(indices.data(), ptr, indices.size()*sizeof(uint16_t));
			ptr = &reinterpret_cast<const uint16_t*>(ptr)[indices.size()];
			mMeshes.emplace_back(vertexData,indices);
		}
		std::vector<std::string> objNames;
		auto str = (const char*)ptr;
		for(size_t i = 0; i < nObjects; ++i)
		{
			auto n = strlen(str);
			objNames.emplace_back(str);
			str += n+1;
		}
		ptr = str;
		mNodes.reserve(nObjects);
		auto objDataList = reinterpret_cast<const RenderObjData*>(ptr);
		for(size_t i = 0; i < nObjects; ++i)
		{
			auto& objSrc = objDataList[i];
			if(objSrc.meshIdx < 0)
				continue;
			mNodes.emplace_back();
			auto& obj = mNodes.back();
			// Object transform
			auto objXForm = new Transform();
			objXForm->matrix() = objSrc.transform.block<3,4>(0,0);
			obj.addComponent(objXForm);
			// Object mesh
			auto& mesh = mMeshes[objSrc.meshIdx];
			auto meshRenderer = mGraphicsScene.createMeshRenderer(&mesh);
			obj.addComponent(meshRenderer);
			obj.init();
		}
	}

	//------------------------------------------------------------------------------------------------------------------
	bool Player::update() {
		core::Time::get()->update();
		auto dt = core::Time::get()->frameTime();
		t += dt;
		if(!mGfxDriver)
			return true;

		gui::startFrame();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		auto camera = AffineTransform::identity();
		camera.setRotation(math::Quatf(Vec3f(0.f,0.f,1.f), t*0.2f));
		camera.position().z() = 120.f;
		camera.position().x() = 400.f;
		auto view = Mat44f::identity();
		view.block<3,4>(0,0) = camera.inverse().matrix();
		auto vp = mProjectionMtx * view;
		auto worldMatrix = Mat44f::identity();

		Vec4f lightDir = { 0.2f, -0.3f, 2.0f , 0.0f };
		mShader->bind();

		for(auto renderable : mGraphicsScene.renderables()) {
			auto& renderObj = renderable->renderObj();
			// Get world matrix
			worldMatrix.block<3,4>(0,0) = renderObj.transform.matrix();
			// Set up wvp
			auto wvp = vp*worldMatrix;
			glUniformMatrix4fv(0, 1, !Mat44f::is_col_major, wvp.data());
			auto& worldI = worldMatrix.transpose();
			auto msLightDir = worldI * lightDir;
			auto msViewDir = worldI.block<3,3>(0,0) * camera.position() + worldI.block<3,4>(0,0).col(3);
			glUniform3f(1, msLightDir.x(), msLightDir.y(), msLightDir.z());
			glUniform3f(2, msViewDir.x(), msViewDir.y(), msViewDir.z());
			// render
			renderObj.mesh->render();
		}

		gui::finishFrame(dt);
		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev