#pragma once
#include <gl/GL.h>

class CGLMaterial
{
public:
	CGLMaterial();
	virtual ~CGLMaterial();

	void Select(GLenum face = GL_FRONT);

	void SetAmbient(float r = 0.2, float g = 0.2, float b = 0.2, float a = 1);
	void SetDiffuse(float r = 0.8, float g = 0.8, float b = 0.8, float a = 1);
	void SetEmission(float r = 0, float g = 0, float b = 0, float a = 1);
	void SetSpecular(float r = 1, float g = 1, float b = 1, float a = 1);
	void SetShininess(float s = 0);

protected:
	float m_vAmbient[4];
	float m_vDiffuse[4];
	float m_vSpecular[4];
	float m_vEmission[4];
	float m_fShininess;
};

