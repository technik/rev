////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Revolution Engine, Shader tool
// Created by Carmelo J. Fernández-Agüera Tortosa (a.k.a. Technik)
// on April 12th, 2012
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// Shader material

#ifndef _SHADERTOOL_SHADERRENDER_SHADERMATERIAL_H_
#define _SHADERTOOL_SHADERRENDER_SHADERMATERIAL_H_

#include <revVideo/material/material.h>

class ShaderMaterial : public rev::video::IMaterial
{
public:
	ShaderMaterial(const char * _pixelShader);
	~ShaderMaterial();

	void	setEnvironment () const;
	void	resetTime();
private:
	mutable float mTime;
};

//------------------------------------------------------------------------------------------------------------------
inline void ShaderMaterial::resetTime()
{
	mTime = 0.f;
}

#endif // _SHADERTOOL_SHADERRENDER_SHADERMATERIAL_H_
