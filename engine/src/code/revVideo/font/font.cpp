////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine
// by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// Created on May 2nd, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Fonts

#include "font.h"

#include <revCore/math/vector.h>
#include <revVideo/scene/model/staticModel.h>
#include <revVideo/texture/texture.h>

namespace rev { namespace video
{
	//------------------------------------------------------------------------------------------------------------------
	CFont::CFont(const char * _filename)
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CFont::~CFont()
	{
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel * CFont::createText(const char * _text, unsigned _size)
	{
		getFontBySize(_size)->createText(_text);
	}

	//------------------------------------------------------------------------------------------------------------------
	CTexture * CFont::getTexture(unsigned _size)
	{
		getFontBySize(_size)->texture();
	}

	//------------------------------------------------------------------------------------------------------------------
	CFont::CSizedFont * CFont::getFontBySize(unsigned _size)
	{
		rtl::map<unsigned,CSizedFont*>::iterator i = mFonts.find(_size);
		if(i == mFonts.end())
		{
			CSizedFont * font = new CSizedFont();
			mFonts[_size] = font;
			return font;
		}
		return i->second;
	}

	//------------------------------------------------------------------------------------------------------------------
	CFont::CSizedFont::CSizedFont(unsigned _size)
	{
		// Create the texture
		mTexture = new CTexture();
	}

	//------------------------------------------------------------------------------------------------------------------
	CFont::CSizedFont::~CSizedFont()
	{
		// Destructor
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel * CFont::CSizedFont::createText(const char * _text)
	{
		CStaticModel * textModel = new CStaticModel();
		unsigned short * indices = new unsigned short[4];
		CVec3 * verts = new CVec3[4];
		verts[0] = CVec3(0.f);
		verts[1] = CVec3(0.f, 10.f, 0.f);
		verts[2] = CVec3(10.f, 0.f, 0.f);
		verts[3] = CVec3(10.f, 10.f, 0.f);
		CVec3 * norms = new CVec3[4];
		for(unsigned i = 0; i < 4; ++i)
		{
			indices[i] = i;
			norms[i] = CVec3(0.f,0.f,1.f);
		}
		CVec2 * uvs = new CVec2[4];
		uvs[0] = CVec2(0.f, 0.f);
		uvs[1] = CVec2(0.f, 1.f);
		uvs[2] = CVec2(1.f, 0.f);
		uvs[3] = CVec2(1.f, 1.f);
		textModel->setVertexData(4, reinterpret_cast<float*>(verts),
									reinterpret_cast<float*>(norms),
									reinterpret_cast<float*>(uvs));
		textModel->setFaceIndices(4, indices, true);
		return textModel;
	}

	//------------------------------------------------------------------------------------------------------------------
	CTexture * CFont::CSizedFont::texture()
	{
		return 0;
	}
}	// namespace video
}	// namespace rev
