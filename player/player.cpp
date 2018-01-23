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
#include <game/scene/camera.h>

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
			// -- triangle --
			mGameProject.load("sample.prj");
			createCamera();
			loadScene("sponza_crytek.scn");

			mRenderer.init();
			gui::init(_window->size);			
		}
		return mGfxDriver != nullptr;
	}

	//------------------------------------------------------------------------------------------------------------------
	void Player::createCamera() {
		// Node
		mNodes.push_back(SceneNode());
		auto& cameraNode = mNodes.back();
		cameraNode.name = "Camera";
		// Transform
		auto objXForm = new Transform();
		objXForm->matrix().setIdentity();
		objXForm->xForm.position() = math::Vec3f { 400.f, 120.f, 170.f };
		objXForm->xForm.setRotation(math::Quatf(Vec3f(0.f,0.f,1.f), 1.57f));
		cameraNode.addComponent(objXForm);
		// Actual camera
		auto camComponent = new game::Camera();
		mCamera = &camComponent->cam();
		cameraNode.addComponent(camComponent);
		// Init camera
		cameraNode.init();
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
			obj.name = objNames[i];
			// Object transform
			auto objXForm = new Transform();
			objXForm->matrix().setIdentity();
			//objXForm->matrix() = objSrc.transform.block<3,4>(0,0);
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
		if(!mGfxDriver)
			return true;
		core::Time::get()->update();
		gui::startFrame();
		mGameEditor.update(mNodes);

		auto dt = core::Time::get()->frameTime();

		for(auto& obj : mNodes)
			obj.update(dt);

		mGraphicsScene.showDebugInfo();
		mRenderer.render(*mCamera, mGraphicsScene);

		gui::finishFrame(dt);
		mGfxDriver->swapBuffers();

		return true;
	}

}	// namespace rev