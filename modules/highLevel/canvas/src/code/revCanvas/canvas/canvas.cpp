//----------------------------------------------------------------------------------------------------------------------
// Revolution SDK
// Created by Carmelo J. Fdez-Agüera Tortosa a.k.a. (Technik)
// On January 30th, 2013
//----------------------------------------------------------------------------------------------------------------------
// Simple canvas

#include "canvas.h"
#include <revVideo/driver3d/driver3d.h>
#include <revVideo/types/color/color.h>
#include <revVideo/types/shader/pixel/pxlShader.h>
#include <revVideo/types/shader/shader.h>
#include <revVideo/types/shader/vertex/vtxShader.h>
#include <revVideo/types/window/videoWindow.h>
#include <revVideo/videoDriver/videoDriver.h>

#include <revVideo/types/shader/vertex/openGL21/vtxShaderOpenGL21.h>

using namespace rev::video;

namespace rev { namespace canvas {
	//-------------------------------------------------------------
	Canvas::Canvas()
	{
		mDriver3d = VideoDriver::getDriver3d();
		// Create shader
		VtxShader * vtxShader = VtxShader::get("canvasShader.vtx");
		// ----- WARNING: Highly platform dependent code -----
		VtxShaderOpenGL21* vtx21 = static_cast<VtxShaderOpenGL21*>(vtxShader);
		vtx21->addAttribute("vertex");
		// ----- End of platform dependent code --------------
		PxlShader * pxlShader = PxlShader::get("canvasShader.pxl");
		mShader = Shader::get(std::make_pair(vtxShader, pxlShader));
		mColorUniformId = mShader->getUniformLocation("color");
		mDriver3d->setShader(mShader);
		// Set screen size uniforms
		int screenSizeUniform = mShader->getUniformLocation("screenSize");
		math::Vec2u screenSize = VideoDriver::get()->mainWindow()->resolution();
		math::Vec2f floatScreenSize = math::Vec2f(float(screenSize.x), float(screenSize.y));
		mDriver3d->setUniform(screenSizeUniform, floatScreenSize);

		mRectIndices[0] = 0;
		mRectIndices[1] = 1;
		mRectIndices[2] = 2;
		mRectIndices[3] = 2;
		mRectIndices[4] = 1;
		mRectIndices[5] = 3;
	}
	
	//------------------------------------------------------------
	void Canvas::rect(const math::Vec2f& _pos, const math::Vec2f& _size)
	{
		mDriver3d->setShader(mShader);
		math::Vec2f vertices[4] = 
		{ _pos, _pos+math::Vec2f(_size.x, 0.f),
		_pos+math::Vec2f(0.f, _size.y), _pos+_size };
		mDriver3d->setAttribBuffer(0, 4, vertices);
		mDriver3d->drawIndexBuffer(6, mRectIndices, Driver3d::EPrimitiveType::triangles);
	}
	
	//------------------------------------------------------------
	void Canvas::line(const math::Vec2f& _start, const math::Vec2f& _end)
	{
		mDriver3d->setShader(mShader);
		math::Vec2f vertices[2] = { _start, _end };
		mDriver3d->setAttribBuffer(0, 2, vertices);
		mDriver3d->drawIndexBuffer(2, mRectIndices, Driver3d::EPrimitiveType::lines);
	}

	//------------------------------------------------------------
	void Canvas::setDrawColor(const video::Color& _color)
	{
		mDriver3d->setShader(mShader);
		mDriver3d->setUniform(mColorUniformId, _color);
	}

}	// namespace canvas
}	// namespace rev