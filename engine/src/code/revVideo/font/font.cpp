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

namespace rev {
	// Static data
	video::CFont::managerT * video::CFont::sManager = 0;

	namespace video
{

	//------------------------------------------------------------------------------------------------------------------
//	CFont::CFont(const string& _filename)
//	{
//	}
//
//	//------------------------------------------------------------------------------------------------------------------
//	CFont::~CFont()
//	{
//	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel * CFont::createText(const char * _text, unsigned _size)
	{
		return getFontBySize(_size)->createText(_text);
	}

	//------------------------------------------------------------------------------------------------------------------
	CTexture * CFont::getTexture(unsigned _size)
	{
		return getFontBySize(_size)->texture();
	}

	//------------------------------------------------------------------------------------------------------------------
//	CFont::CSizedFont * CFont::getFontBySize(unsigned _size)
//	{
//		rtl::map<unsigned,CSizedFont*>::iterator i = mFonts.find(_size);
//		if(i == mFonts.end())
//		{
//			CSizedFont * font = new CSizedFont();
//			mFonts[_size] = font;
//			return font;
//		}
//		return i->second;
//	}

	//------------------------------------------------------------------------------------------------------------------
//	CFont::CSizedFont::CSizedFont(unsigned _size)
//	{
//		// Create the texture
//		// Create a new buffer filled with the Xor texture
//		u8 * buffer = new u8[4*32*32];
//		for(unsigned i = 0; i < 32; ++i)
//			for(unsigned j = 0; j < 32; ++j)
//			{
//				buffer[4*(i+32*j)+0] = (i ^ j) & 0xff; // r
//				buffer[4*(i+32*j)+1] = (i ^ j) & 0xff; // g
//				buffer[4*(i+32*j)+2] = (i ^ j) & 0xff; // b
//				buffer[4*(i+32*j)+3] = 0xff; // alpha
//			}
//
//		mTexture = new CTexture(buffer, 32, 32);
//	}

	//------------------------------------------------------------------------------------------------------------------
	CFont::CSizedFont::~CSizedFont()
	{
		delete mTexture;
	}

	//------------------------------------------------------------------------------------------------------------------
	CStaticModel * CFont::CSizedFont::createText(const char * _text)
	{
		_text;
		// Vertex data
		CStaticModel * textModel = new CStaticModel();
		CVec3 * verts = new CVec3[4];
		verts[0] = CVec3(0.f);
		verts[1] = CVec3(0.f, 10.f, 0.f);
		verts[2] = CVec3(10.f, 0.f, 0.f);
		verts[3] = CVec3(10.f, 10.f, 0.f);
		CVec3 * norms = new CVec3[4];
		for(unsigned i = 0; i < 4; ++i)
		{
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
		// Indices
		unsigned short * indices = new unsigned short[6];
		indices[0] = 0;
		indices[0] = 1;
		indices[0] = 2;
		indices[0] = 0;
		indices[0] = 2;
		indices[0] = 3;
		textModel->setFaceIndices(4, indices, true);
		return textModel;
	}

	//------------------------------------------------------------------------------------------------------------------
	CTexture * CFont::CSizedFont::texture()
	{
		return mTexture;
	}
}	// namespace video
}	// namespace rev
