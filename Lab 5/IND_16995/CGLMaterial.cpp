#include "pch.h"
#include "CGLMaterial.h"

CGLMaterial::CGLMaterial()
{
	long size = sizeof(float) * 4;
	memset(this->m_vAmbient, .2, size);
	memset(this->m_vDiffuse, .8, size);
	memset(this->m_vEmission, 0, size);
	memset(this->m_vSpecular, 1, size);
	this->m_fShininess = 0;
}

CGLMaterial::~CGLMaterial()
{}

void CGLMaterial::Select(GLenum face)
{
	glMaterialfv(face, GL_AMBIENT, this->m_vAmbient);
	glMaterialfv(face, GL_DIFFUSE, this->m_vDiffuse);
	glMaterialfv(face, GL_EMISSION, this->m_vEmission);
	glMaterialfv(face, GL_SPECULAR, this->m_vSpecular);
	glMaterialf(face, GL_SHININESS, this->m_fShininess);
}

void CGLMaterial::SetAmbient(float r, float g, float b, float a)
{
	this->m_vAmbient[0] = r;
	this->m_vAmbient[1] = g;
	this->m_vAmbient[2] = b;
	this->m_vAmbient[3] = a;
}

void CGLMaterial::SetDiffuse(float r, float g, float b, float a)
{
	this->m_vDiffuse[0] = r;
	this->m_vDiffuse[1] = g;
	this->m_vDiffuse[2] = b;
	this->m_vDiffuse[3] = a;
}

void CGLMaterial::SetEmission(float r, float g, float b, float a)
{
	this->m_vEmission[0] = r;
	this->m_vEmission[1] = g;
	this->m_vEmission[2] = b;
	this->m_vEmission[3] = a;
}

void CGLMaterial::SetSpecular(float r, float g, float b, float a)
{
	this->m_vSpecular[0] = r;
	this->m_vSpecular[1] = g;
	this->m_vSpecular[2] = b;
	this->m_vSpecular[3] = a;
}

void CGLMaterial::SetShininess(float s)
{
	this->m_fShininess = min(max(s, 0), 128);
}
